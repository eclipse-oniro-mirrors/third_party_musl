# pthread_key 迁移到 thread_local 指南

本文面向 OpenHarmony 原生 C++ 模块开发者，介绍如何将固定用途的 `pthread_key_t` 线程私有数据迁移为 C++11 `thread_local`，减少业务重复创建 key 的情况，并明确迁移时的生命周期和资源清理要求。

## 1. 迁移目的与适用范围

当前 musl 的 `PTHREAD_KEYS_MAX` 为 128，表示整个进程同时存在的 key 上限。应用、运行时和三方库共享这份额度。一个 key 可以被多个线程共用，各线程保存自己的值，因此不应为同一用途在每个线程或每次请求中重新创建 key。

对于编译时已知、需要每个线程独立保存的状态，可以用 `thread_local` 变量代替业务显式创建的 key。每个线程拥有独立实例，无需业务调用 `pthread_key_create()`、`pthread_getspecific()` 和 `pthread_setspecific()`。

| 业务场景 | 迁移建议 |
| --- | --- |
| C++ 模块固定的每线程计数、缓存或上下文 | 可以评估迁移，通过统一访问函数取得当前线程实例 |
| 对象拥有内存、文件或其他资源 | 使用有明确析构逻辑的类型或智能指针，不能只替换为裸指针变量 |
| 需要运行时动态创建任意数量的 key，或由外部调用者传入 key | 保留原接口或重新设计上下文管理，无法直接替换为固定变量 |
| 纯 C 模块 | C++ `thread_local` 不能直接写入 C 源文件；可评估 `_Thread_local` 或 C++ 封装，但清理规则需要单独设计 |
| 线程池任务、协程或可能跨线程调度的任务上下文 | 先确认真正需要的是线程状态还是任务状态；`thread_local` 绑定执行线程，不会自动随任务迁移 |

这里减少的是业务显式申请的 key。当前 [`cxa_thread_atexit_impl.c`](../../../src/exit/cxa_thread_atexit_impl.c) 保留 `CXA_THREAD_USE_TSD` 和 `CXA_THREAD_USE_TLS` 两条实现路径：TSD 路径使用一个内部 key 管理 C++ TLS 析构，并非每个 `thread_local` 对象各创建一个 key。因此，不能将迁移理解为整个进程不再使用 pthread key，或完全没有运行时开销。

## 2. 前后对照示例

下面的上下文只保存一个计数，不拥有额外资源。示例适用于 C++11 及以上。

### 2.1 迁移前：通过 pthread key 访问

```cpp
#include <cerrno>
#include <new>
#include <pthread.h>

struct ThreadContext {
    unsigned int requestCount;
};

static pthread_once_t g_once = PTHREAD_ONCE_INIT;
static pthread_key_t g_contextKey;
static int g_keyError;

static void DestroyContext(void *value)
{
    delete static_cast<ThreadContext *>(value);
}

static void CreateContextKey()
{
    g_keyError = pthread_key_create(&g_contextKey, DestroyContext);
}

int GetThreadContext(ThreadContext **out)
{
    if (out == nullptr) {
        return EINVAL;
    }
    *out = nullptr;
    int ret = pthread_once(&g_once, CreateContextKey);
    if (ret != 0 || g_keyError != 0) {
        return ret != 0 ? ret : g_keyError;
    }
    auto *context = static_cast<ThreadContext *>(pthread_getspecific(g_contextKey));
    if (context == nullptr) {
        context = new (std::nothrow) ThreadContext{};
        if (context == nullptr) {
            return ENOMEM;
        }
        ret = pthread_setspecific(g_contextKey, context);
        if (ret != 0) {
            delete context;
            return ret;
        }
    }
    *out = context;
    return 0;
}
```

此处整个模块只申请一个 key，并非每个线程申请一个。示例用 `new/delete` 管理对象，原有业务也应保持分配与释放方式匹配。

### 2.2 迁移后：通过 thread_local 访问

```cpp
#include <cerrno>

struct ThreadContext {
    unsigned int requestCount;
};

int GetThreadContext(ThreadContext **out)
{
    if (out == nullptr) {
        return EINVAL;
    }
    static thread_local ThreadContext context{};
    *out = &context;
    return 0;
}
```

调用方仍通过 `GetThreadContext()` 取得上下文。同一线程多次调用得到同一实例，不同线程得到各自的实例。这里不再需要业务的 key、once 控制对象及相应回调；业务原有的 key 耗尽和堆分配失败路径也随实现改变，不能把此例的行为推广到有复杂构造函数的类型。

这个函数应在一个 `.cpp` 文件中提供唯一实现。不要把带 `static` 内部链接的访问函数复制到多个源文件或公共头文件中，否则可能为同一业务产生多份独立状态。跨动态库调用也应统一入口，不能仅凭相同变量名认为所有模块共享同一份 TLS 定义。

## 3. 资源所有权与主动清理

`thread_local Context *context` 只让指针本身按线程保存，不会在线程退出时自动 `delete` 它指向的对象。若原 key 的析构函数会释放对象，迁移后必须保留等价的资源所有权。

可以直接保存拥有资源的对象，让其析构函数完成清理；若需要按需创建和提前释放，可使用智能指针。例如：

```cpp
#include <memory>
#include <new>

struct Context {
    unsigned int requestCount = 0;
};

static std::unique_ptr<Context>& CurrentContext()
{
    static thread_local std::unique_ptr<Context> context;
    return context;
}

Context *GetOrCreateContext()
{
    auto& context = CurrentContext();
    if (!context) {
        context.reset(new (std::nothrow) Context);
    }
    return context.get();
}

void ResetCurrentContext()
{
    CurrentContext().reset();
}
```

调用方应检查 `GetOrCreateContext()` 是否返回空指针。这里的 `std::nothrow` 处理对象存储分配失败；若业务构造函数本身可能抛异常，还需匹配项目的异常策略。

`ResetCurrentContext()` 只清理调用线程的对象。线程池中，线程可能长期不退出，因此请求结束或租户切换时需要主动重置状态，不能等待线程退出后才清理敏感信息或资源。旧的 `pthread_setspecific(key, nullptr)` 只是清空关联值，不自动释放旧对象；智能指针的 `reset()` 会释放，迁移前应核对旧代码的清理责任，避免重复释放。

若原 key 保存的是外部拥有的"当前上下文"指针，应保留借用关系、设置和清空接口，以及嵌套调用时旧指针的保存恢复；不能直接改成独占对象或智能指针。不同业务实例原先各自创建 key 时，也不能用一个 TLS 变量将它们的状态合并。

## 4. 生命周期与兼容性风险

- **析构语义需要重新核对。** pthread key 在线程退出清理路径中，对已设置的非空值调用登记的析构函数；`pthread_key_delete()` 不调用值的析构。C++ `thread_local` 对已构造的对象按 C++ TLS 规则析构，不能照搬多个 key 的清理顺序或反复写回 key 值的逻辑，也不要依赖 TSD 与 C++ TLS 析构之间固定的先后关系。
- **线程私有不等于可跨线程共享。** 从访问函数返回的指针属于调用线程的实例，不应交给其他线程继续访问或在线程退出后使用；需要共享的数据应另行设计所有权和同步。
- **任务本地适配不能直接替换。** 某些 FFRT 配置会切换任务使用的 TSD 存储，使 key 数据跟随任务。此类路径必须先核对任务本地适配逻辑，不能假定普通 C++ `thread_local` 也随任务切换。
- **迁移不提供进程级撤销操作。** `pthread_key_delete()` 撤销的是进程共享的 key，不会析构关联对象；`thread_local` 没有对应的全进程标识撤销操作，重置也只作用于调用线程。需要提前回收对象时，应由各线程协作，或调整模块生命周期。
- **动态库卸载要验证。** 在 `dlopen/dlclose`、TLS 析构回调和工作线程同时存在时，必须确认目标运行时的卸载策略和模块生命周期，不应假设 `dlclose()` 会立即销毁所有线程的对象。
- **内存和失败路径会变化。** 大型直接 TLS 对象可能增加每线程资源占用；动态初始化和析构登记也可能分配内存。复杂构造和析构不应放入信号处理、分配器重入等敏感路径。按实际线程数评估开销。
- **C 和 C++ TLS 清理规则不同。** C11 `_Thread_local` 或编译器 `__thread` 不提供 C++ 对象的 RAII 析构，不能据此直接替代有清理回调的 key。编译器、目标架构及运行时支持也需要确认。
- **异常退出不保证清理。** 崩溃、强制结束及 `_exit()` 等路径不能依赖常规线程析构释放业务资源。迁移也不改变多线程进程 `fork()` 后、`exec` 前的异步信号安全限制。

## 5. 迁移步骤与验证建议

1. 盘点同一 key 的 create、get、set、delete 和析构回调，确认它表示固定的每线程状态，梳理所有权及主动清理时机。
2. 将访问集中到一个模块入口，再引入 `thread_local`；固定小状态可直接保存对象，需要按需分配或重置的状态可用拥有所有权的封装。
3. 将原 set、替换及清空操作转换为对象赋值、重置或专用接口，逐项核对旧值释放、错误返回与调用方预期。
4. 完成所有调用点迁移后，移除不再使用的业务 key 初始化、once、析构回调和 delete 路径。运行中的旧对象不能只靠删除 key 回收，应安排停用和清理。
5. 在目标产品构建中验证：同线程状态保持、不同线程隔离；线程退出时资源只清理一次；线程池任务切换后的重置；初始化失败；涉及模块的加载和卸载。

迁移收益应按模块显式创建的 key 数量评估。一个供全部线程共用的 key，迁移后减少的是一个业务 key，而不是"线程数个 key"。如果只是重复创建同类 key，也可以先修正为创建一次并复用，再决定是否迁移到 `thread_local`。

## 6. 实现参考

- [key 创建、删除与 TSD 析构](../../../src/thread/pthread_key_create.c)
- [C++ TLS 析构登记](../../../src/exit/cxa_thread_atexit_impl.c)
- [线程退出](../../../src/thread/pthread_create.c)
- [当前 key 上限定义](../../../include/limits.h)
