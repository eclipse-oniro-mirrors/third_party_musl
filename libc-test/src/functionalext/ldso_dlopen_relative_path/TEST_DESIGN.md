# dlopen 相对路径加载矩阵测试设计

## 目标

验证 `dlopen_enable_relative_path()` 开关与路径形态、namespace 可加载范围的组合结果。

## 前置条件

- namespace：`dlns_create2(&ns, allowed, 0)` 且 `separated=true`，只允许 `allowed`，不继承；
- `blocked/` 中的库和符号链接真实存在，确保拒绝来自访问控制而非文件缺失；
- 每个用例分别通过 `dlopen_ns`、`dlopen_ns_ext`、库内 `dlopen`、库内 `dlopen_ext` 执行；
- 开关默认关闭，调用 `dlopen_enable_relative_path()` 后开启且不可关闭。

## 核心矩阵

| 编号 | 路径形态 | 可加载范围 | 开关 | 预期 | 用例 |
| --- | --- | --- | --- | --- | --- |
| M1 | 绝对路径 | 范围内 | 关 | 可加载 | `$ROOT/allowed/librelative_path_fixture.so` |
| M2 | 绝对路径 | 范围内 | 开 | 可加载 | 同上 |
| M3 | 绝对路径 | 范围外 | 关 | 拒绝 | `$ROOT/blocked/librelative_path_fixture.so` |
| M4 | 绝对路径 | 范围外 | 开 | 拒绝 | 同上 |
| M5 | 相对路径 | 范围内 | 关 | 拒绝 | `./allowed/librelative_path_fixture.so` |
| M6 | 相对路径 | 范围内 | 开 | 可加载 | 同上；另含 cold fixture、不带 `./` 的形式、`good.so` |
| M7 | 相对路径 | 范围外 | 关 | 拒绝 | `./blocked/librelative_path_fixture.so`、`./allowed/escape.so` |
| M8 | 相对路径 | 范围外 | 开 | 拒绝 | 同上 |
| M9 | 纯名称 | 搜索路径内 | 关 | 可加载 | `librelative_path_fixture.so` |
| M10 | 纯名称 | 搜索路径内 | 开 | 可加载 | 同上 |
| M11 | 纯名称 | 搜索路径外 | 关 | 拒绝 | `librelative_path_hidden.so`（仅存在于 blocked） |
| M12 | 纯名称 | 搜索路径外 | 开 | 拒绝 | 同上 |

说明：

- M5 是核心约束：相对路径的真实文件即使在允许范围内，开关关闭时也必须拒绝；
- 开关只影响含 `/` 且不以 `/` 开头的路径，绝对路径和纯名称在开关两侧结果一致；
- 纯名称的可加载范围按 namespace 搜索路径判定；
- 特性代码仅在 `musl_extended_function=true` 时编译；矩阵测试在两种配置下都构建，
  未启用时接口仍导出但为空操作，开启侧用例按拒绝预期执行。

## 补充用例

| 场景 | 用例 | 预期 |
| --- | --- | --- |
| 超长路径 | `PATH_MAX + 32` 的相对路径，`RTLD_NOW` | 拒绝，不崩溃 |
| `RTLD_LAZY` | `./allowed/librelative_path_fixture.so`，`RTLD_LAZY` | 与 `RTLD_NOW` 一致 |
| 非零 extinfo | `dlopen_ns_ext` / 库内 `dlopen_ext` 传入 `flag=0`、`relro_fd=-1` | 与零 extinfo 一致 |

## 判定

- 成功：句柄非空，`relative_path_value()` 和 `relative_path_nested_value()` 均为 42，`dlclose()` 为 0；
  冷加载额外校验 `dladdr().dli_fname` 等于调用者传入的路径。
- 失败：句柄为空且 `dlerror()` 非空。
