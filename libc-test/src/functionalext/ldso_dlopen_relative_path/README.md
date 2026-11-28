# dlopen 相对路径测试

对应需求 `20260811011432`。特性仅在 L2 标准系统（`is_standard_system=true`）且
`musl_extended_function=true` 的 Linux 构建中编译；`musl_extended_function=false`
时特性代码不编译，`dlopen_enable_relative_path` 符号仍导出但调用为空操作、无副作用，
相对路径仍按默认策略拒绝。测试组在两种形态下都构建，未使能形态用于验证符号可解析且调用无效果。
测试不添加公共头文件声明。接口说明见
[DLOPEN_ENABLE_RELATIVE_PATH.md](../../../../docs/interface/dynlink/DLOPEN_ENABLE_RELATIVE_PATH.md)。

## 测试目标

| 可执行文件 | 覆盖内容 |
| --- | --- |
| `dlopen_relative_path_matrix` | 三维矩阵：绝对/相对/纯名称 × namespace 范围内/范围外 × 开关开/关，共 12 格；相对路径用冷加载验证首次加载、用符号链接验证按真实路径校验；每个用例执行四个公开入口 |

矩阵设计见 [TEST_DESIGN.md](TEST_DESIGN.md)。未使能构建（`EXPECT_RELATIVE_PATH=0`）
会通过 dlsym 校验符号可解析，并验证调用使能接口后相对路径仍被拒绝（no-op、无副作用）。
补充回归：`PATH_MAX+32` 超长相对路径拒绝、`RTLD_LAZY` 加载、非零 `extinfo`
经 `dlopen_ns_ext` 和库内 `dlopen_ext` 透传。
并发与符号用例的源码保留在目录中，当前未注册构建目标，后续可恢复。

矩阵测试通过 `dlopen_ns`、`dlopen_ns_ext` 和测试库内的 `dlopen`、`dlopen_ext`
调用，覆盖四个公开入口。测试库内调用用于检查调用者命名空间选择。

测试动态创建 separated 命名空间，不设置继承，只允许临时目录中的 `allowed`
目录。`blocked` 中的库和指向它的符号链接均真实存在，保证拒绝结果来自访问控制，
而非文件缺失。纯名称用例中仅存在于 `blocked` 的库用于验证搜索路径外的名称不可加载。

## fd 解析与加载顺序

按最新实现约定，在 `load_library_header` 的普通文件分支处理相对路径；
未启用加载随机化时，在 `load_library` 的对应分支使用相同逻辑。
编译宏和运行时开关均开启且参数是含 `/` 的相对路径时：

1. `open(name, O_RDONLY | O_CLOEXEC)`；失败则继续原有失败和继承查找流程。
2. 通过 `readlink("/proc/self/fd/<fd>", ...)` 获取已打开文件的绝对路径。
3. 使用该绝对路径执行 `is_accessible`；解析失败、截断、非绝对结果或权限拒绝均关闭 fd。
4. 校验通过后沿用同一 fd 加载，不按解析结果重新打开文件。

随机化分支复用堆上 `task->buf`，非随机化分支复用已有局部 `buf`。
新增逻辑不调用 `realpath`、不新增 `PATH_MAX` 栈数组或静态路径缓冲；
相对路径辅助函数内置同样的 `/proc/self/fd` 解析，原有 `fd_to_realpath` 的位置和实现保持不变，
不能将这条调用链理解为零栈占用。
`dlopen_impl` 已恢复原有入口逻辑、取消控制、加锁位置及 ASAN 重定向顺序。
开关关闭时沿用原有访问检查和打开顺序；纯文件名、绝对路径和 ZIP 专用分支保持原逻辑。

DSO 的 `name` 保持调用者传入的路径：显式相对路径按原样保存，解析出的绝对路径只用于命名空间校验，
`dladdr` 返回的 `dli_fname` 也是调用者传入的路径。`pathname` 与 `name` 仍为同一指针，
原有 `shortname` 判定自然成立，显式路径不会给 DSO 额外设置 basename 索引。
ADLT 的 `fullname` 保留原始符号链接名称以查找子库。
冷加载测试库的构造函数会再次加载另一个相对路径，验证嵌套加载后的 DSO 名称。

## 构建

在完整 OpenHarmony 源码根目录执行；分别使用 `musl_extended_function=true/false` 的构建产物：

```sh
./build.sh --product-name rk3568 --build-target musl_all --gn-args musl_extended_function=true
./build.sh --product-name rk3568 --build-target //third_party/musl/libc-test/src/functionalext/ldso_dlopen_relative_path:ldso_dlopen_relative_path_test --gn-args musl_extended_function=true
```

另一组将 `musl_extended_function` 改为 `false`。测试程序的 `EXPECT_RELATIVE_PATH`
由相同 GN 参数生成（true 为 1，false 为 0），必须与运行设备的 libc 配置一致，
不要在同一设备上混用两种配置的产物。

该测试组在 `test_src_functionalext.gni` 中仅向 L2 注册，两种扩展功能配置都会构建，
会进入 L2 的 `libctest` 构建。测试目录还检查 Linux 加载器是否存在；
Cortex-M 和 riscv32 不生成这些测试目标。

## 设备运行

运行前部署对应配置的 libc，并按 libc-test 常规部署流程准备测试程序与测试库。
测试库默认位置为 `/data/local/tmp/libc-test-lib/librelative_path_fixture.so`。
运行账号需要能在 `/data/local/tmp` 创建临时目录，并有权限调用命名空间创建接口。

```sh
./dlopen_relative_path_matrix
```

矩阵程序接受测试库路径作为第一个参数。每次运行创建独立临时目录，正常结束时清理；
前置条件缺失会返回非零状态，不会跳过后报告通过。

正式产物应在使能/未使能两种构建下检查动态导出，两种形态均应包含该符号：

```sh
llvm-readelf --dyn-syms libc.so | grep dlopen_enable_relative_path
```

## 本次本地验证及边界

- 测试源码使用本仓头文件，在下列 13 种架构下按开关开/关交叉编译，开启 `-Wall -Wextra -Werror`：
  aarch64、arm、x86_64、mips（小端）、riscv64、loongarch64、i386、x32、mips64（小端）、
  mipsn32（小端）、powerpc、powerpc64（小端）、s390x。
- `dynlink.c` 在上述 13 种架构下覆盖相对路径宏开/关、加载随机化开/关；扩展功能开启时
  同时定义 `MUSL_EXTERNAL_FUNCTION` 和 `FEATURE_PTHREAD_CANCEL`，与实际 GN 注入保持一致。
  使用本地 LLVM 和 SDK 的 FreeBSD `sys/queue.h`；针对原有代码的指针转换告警使用兼容选项，未修改原有代码。
  另验证 arm/aarch64 的 ASAN、aarch64/x86_64 的 TSAN、aarch64 的 HWASAN，
  以及 arm/aarch64/x86_64 的 `UNIT_TEST_STATIC + IO_GUARD_ENABLE` 条件编译，共 68 组通过。
  这些是条件编译检查，不代表 sanitizer 设备运行验证。
- 两种配置的目标文件均含新符号；`true` 配置启用开关状态，`false` 配置函数体为空操作。
  新增加载逻辑在宏关闭配置中不引入 `realpath` 引用。
  13 种架构的 26 组 extern 调用对象与 loader 对象可重定位链接通过，13 个测试动态库链接通过；
  这不等同于正式 libc.so 的动态导出验证。
- GN 中已映射的 Linux 动态加载器架构为 arm、aarch64、x86_64、mips、riscv64、loongarch64；
  其余架构为额外源码编译检查，不新增产品支持。Cortex-M/riscv32 的该加载器依赖由构建模板排除。
  当前 LLVM 没有 m68k、microblaze、or1k、sh 后端，这四种架构未执行编译验证。
- 已核对功能宏受 L2/Linux/扩展功能三个条件限制，Lite 构建脚本和 porting 副本均未引入该宏。
- 本机辅助检查提取实际 fd 解析函数及随机化/非随机化普通文件分支，在两种宏配置下运行，
  开启 AddressSanitizer、UndefinedBehaviorSanitizer。覆盖开关关闭、成功加载、打开失败、
  解析失败/截断/空串/非绝对路径、反复拒绝后的 fd 释放（包括 fd=0），以及校验时替换符号链接后
  仍保留原 fd。macOS 上使用 F_GETPATH 适配 proc fd 路径读取、模拟 namespace 判定；
  使用真实 open/close/read 验证 fd 行为，不执行完整 OpenHarmony 动态链接器，也不代表设备测试。
- 本机缺少完整 OpenHarmony 构建根；完整 GN/libctest 构建、设备矩阵运行、长稳、
  actsToolchainTest、actsThirdPartyMuslTest 和正式 libc.so/NDK 暴露检查仍需在相应环境完成。
