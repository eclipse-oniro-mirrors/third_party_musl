# malloc 分配器知识

本文只记录 malloc 分配器子系统的框架设计、OH 新增功能、专有名词和修改约束。
fortify/GWP-ASan/fdsan 的整体关系见 [[memory-safety]]，LD_PRELOAD 替换与加载器的交互见 [[dynlink]]。

## 框架设计

### 调用链分层（OH 构建，HOOK_ENABLE）

```
用户代码 malloc()
  → src/hook/linux/malloc_common.c    公共符号定义 + 分发层
      ├─ dispatch table 生效          → hook so / memleak tracker 函数表
      ├─ USE_GWP_ASAN                → GWP-ASan 采样分配（见 [[memory-safety]]）
      └─ 默认                         → libc 内建分配器（mallocng）
```

非 HOOK_ENABLE 构建（ASan 构建、mips）无分发层，公共符号直接来自分配器本身。

### 三种分配器

| 分配器 | 目录 | 来源 | 选择方式 |
|---|---|---|---|
| mallocng | `src/malloc/mallocng/` | 上游 musl 默认（meta/group 元数据分配器） | 默认（Makefile `MALLOC_DIR`） |
| oldmalloc | `src/malloc/oldmalloc/` | musl 经典 dlmalloc 风格（chunk/bins） | `--with-malloc=oldmalloc` |
| jemalloc | `src/malloc/jemalloc/linux/` | 外部 jemalloc 集成 | `USE_JEMALLOC` 编译宏 |

分配器通过编译期改名接入：mallocng 的 malloc/free/realloc 编译为 `__libc_malloc_impl` /
`__libc_free` / `__libc_realloc`，公共符号由分发层（HOOK_ENABLE 构建）或弱别名提供。

### mallocng 顶层设计

- 48 个 size class（16 字节单位）+ **MMAP_THRESHOLD**（约 128KB）以上独立 mmap
- 分配基本单元是 **group**：一块连续内存切分的等大 slot 数组，元数据（**meta**）每 group
  一个、单独存放，通过 slot 位图（avail/freed mask）管理分配状态
- 用户指针前 4 字节是 out-of-band 头部，记录 slot 归属与大小；free/malloc_usable_size
  依赖头部反查 meta，头部布局是分配器内部 ABI
- 小分配走免锁快路径（位图 CAS），组创建/回收走全局锁慢路径；fork 由 `__malloc_atfork`
  在 prepare/parent/child 三态加锁/解锁/重置处理
- calloc 利用新映射内存天然为 0 的特性跳过不必要的清零

## OH 新增功能点

| 功能 | 位置 | 说明 |
|---|---|---|
| hook 分发层 | `src/hook/linux/malloc_common.c` | 公共 malloc/free/realloc/calloc/mmap/munmap/prctl 符号统一入口，按 dispatch table 与开关分发 |
| 可插拔 dispatch table | `src/hook/linux/musl_malloc_dispatch.h` | 分配器函数表（分配/释放 + DFX + trace 钩子），原子切换 |
| hook so 加载 | `src/hook/linux/musl_preinit.c` | libnative_hook.z.so / memleak tracker，startup/direct/step 三模式安装 |
| 安全加固开关 | `musl_template.gni`（musl_secure_level 分层） | FREELIST_HARDENED / QUARANTINE / RED_ZONE / SECURE_ALL 逐级开启；QUARANTINE/RED_ZONE 当前仅有配置管道（依赖校验），实现未合入 |
| VMA 命名 | `src/malloc/mallocng/` | prctl(PR_SET_VMA) 把分配器 VMA 命名为 native_heap:*，方便 DFX 定位 |
| jemalloc DFX 接口 | `src/malloc/linux/stats.c` | mallinfo/malloc_stats_print/malloc_iterate 等转发 je_* |
| 大分配上报 | `src/hook/linux/malloc_common.c` | 单次分配 ≥300MB 时 raise 信号通知资源追踪模块 |

## 专有名词

| 名词 | 定义 | 与上游 musl 的关系 |
|---|---|---|
| **mallocng** | musl 新版分配器：meta/group 位图 + size class + out-of-band 头部 | musl 原有，OH 加安全加固 |
| **oldmalloc** | musl 经典 dlmalloc 风格分配器，chunk header + bins | musl 原有 |
| **jemalloc** | 第三方高性能分配器，编译期集成 | 第三方，wrapper 接入 |
| **group / meta / meta_area / size class** | 分配载体 / 元数据 / 元数据页 / 48 档几何递增规格 | musl 原有 |
| **MMAP_THRESHOLD** | 大分配阈值（约 128KB），以上独立 mmap | musl 原有 |
| **MALLOC_FREELIST_HARDENED** | meta 指针 XOR 加密 + 链表一致性校验 | OH 安全加固 |
| **MALLOC_FREELIST_QUARANTINE / MALLOC_RED_ZONE** | 释放隔离 / 红区检测开关 | OH 安全加固 |
| **MALLOC_SECURE_ALL** | 槽位随机化（/dev/urandom） | OH 安全加固 |
| **MallocDispatchType / dispatch table** | 可插拔分配器函数表及当前生效表 | OH 独有 |
| **MuslFunc / MuslMalloc** | 宏：HOOK_ENABLE 下指向 libc 内建实现，jemalloc 下指向 je_* | OH 独有 |
| **hook so / memleak tracker** | libnative_hook.z.so / libmemleak_tracker.so，分配监控注入点 | OH 独有 |
| **hook_mode** | startup/direct/step 三种 hook 安装模式，param libc.hook_mode 控制 | OH 独有 |
| **MALLOC_REPORT_LIMIT** | 大分配上报阈值（300MB） | OH 独有 |
| **__malloc_replaced / __aligned_alloc_replaced** | LD_PRELOAD 替换标记 | musl 原有 |

## 约束

- **公共符号归属**：HOOK_ENABLE 构建中 malloc/free 等公共符号只能由分发层提供，分配器源码不可直接定义
- **不可破坏 LD_PRELOAD 替换路径**：`__malloc_replaced` 置位后 libc 内部分配走替换后的分配器；
  libc 的 aligned_alloc 在部分替换场景下直接 ENOMEM，防止跨分配器混用
- **分配器只能编译期选择**（`--with-malloc=` / `USE_JEMALLOC`），运行时不可切换
- **安全开关依赖顺序不可破坏**：QUARANTINE 依赖 HARDENED，RED_ZONE 依赖 QUARANTINE（#error 校验）
- **dispatch table 必须原子读写**；hook 安装须在独立线程完成，避免 malloc 调用栈内递归 dlopen
- **out-of-band 头部布局是分配器内部 ABI**，free/malloc_usable_size 等均依赖，不可随意改动
- **不可修改 malloc/free/realloc/calloc 等公共 API 的行为语义**
