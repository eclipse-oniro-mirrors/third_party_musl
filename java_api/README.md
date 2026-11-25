# Java 版 C 标准库接口（实验）

> # ⚠️ 状态：想法验证阶段（Proof of Concept）
>
> **本目录内容当前仅用于想法验证 / 原型探索，尚未经过评审与验证，不可在生产中使用。**
>
> - 语义映射、包结构、命名、返回类型约定等均可能后续调整；
> - 未接入任何编译流程与发布流程，无 ABI/API 稳定性承诺；
> - 请勿在任何正式产品、正式代码路径中依赖本目录的任何类或方法。

---

本目录提供一组从 musl libc 选取的、**独立性高**的 C 接口的 Java 等价实现，用于
C → Java 接口转换的探索与演示。

> 说明：本目录为实验性质，**尚未接入现有编译流程**（BUILD.gn / Makefile 等），
> 仅作为独立 Java 源码存在。

## 选取原则

仅选取满足以下条件的接口，保证转换边界清晰、无隐式依赖：

- **纯函数**：无全局可变状态、无 I/O、无副作用；
- **确定性**：相同输入恒得相同输出；
- **无内存所有权**：不涉及裸指针的分配 / 释放转移；
- **无上下文依赖**：不依赖 fd、线程、信号、errno、locale 等运行时状态。

因此规避了 `malloc`/`free`、`open`/`read`/`write`、`printf`、`errno` 等不适合直接映射的接口。

## 接口清单与映射（共 123 个）

### StringFuncs —— `<string.h>` / `<strings.h>`（25 个）

| C 接口 | 头文件 | Java 方法 | 返回类型映射 |
|---|---|---|---|
| `size_t strlen(const char *)` | string.h | `strlen(String)` | `size_t` → `int` |
| `size_t strnlen(const char *, size_t)` | string.h | `strnlen(String, int)` | `size_t` → `int` |
| `int strcmp(const char *, const char *)` | string.h | `strcmp(String, String)` | `int` |
| `int strncmp(const char *, const char *, size_t)` | string.h | `strncmp(String, String, int)` | `int` |
| `int strcasecmp(const char *, const char *)` | strings.h | `strcasecmp(String, String)` | `int` |
| `int strncasecmp(const char *, const char *, size_t)` | strings.h | `strncasecmp(String, String, int)` | `int` |
| `int memcmp(const void *, const void *, size_t)` | string.h | `memcmp(byte[], byte[], int)` | `void*` → `byte[]` |
| `void *memchr(const void *, int, size_t)` | string.h | `memchr(byte[], int, int)` | 指针 → 下标，`NULL` → `-1` |
| `void *memcpy(void *, const void *, size_t)` | string.h | `memcpy(byte[], byte[], int)` | `void*` 返回值省略 → `void` |
| `void *memmove(void *, const void *, size_t)` | string.h | `memmove(byte[], byte[], int)` | `void*` 返回值省略 → `void` |
| `void *memset(void *, int, size_t)` | string.h | `memset(byte[], int, int)` | `void*` 返回值省略 → `void` |
| `char *strchr(const char *, int)` | string.h | `strchr(String, int)` | 指针 → 下标，`NULL` → `-1` |
| `char *strrchr(const char *, int)` | string.h | `strrchr(String, int)` | 指针 → 下标，`NULL` → `-1` |
| `char *strstr(const char *, const char *)` | string.h | `strstr(String, String)` | 指针 → 下标，`NULL` → `-1` |
| `char *strpbrk(const char *, const char *)` | string.h | `strpbrk(String, String)` | 指针 → 下标，`NULL` → `-1` |
| `char *strchrnul(const char *, int)` | string.h | `strchrnul(String, int)` | 指针 → 下标，结尾 NUL → `len` |
| `size_t strspn(const char *, const char *)` | string.h | `strspn(String, String)` | `size_t` → `int` |
| `size_t strcspn(const char *, const char *)` | string.h | `strcspn(String, String)` | `size_t` → `int` |
| `void *memmem(const void *, size_t, const void *, size_t)` | string.h | `memmem(byte[], int, byte[], int)` | 指针 → 下标，`NULL` → `-1` |
| `void *memrchr(const void *, int, size_t)` | string.h | `memrchr(byte[], int, int)` | 指针 → 下标，`NULL` → `-1` |
| `void *rawmemchr(const void *, int)` | string.h | `rawmemchr(byte[], int)` | 指针 → 下标，未找到 → `-1` |
| `char *strcasestr(const char *, const char *)` | string.h | `strcasestr(String, String)` | 指针 → 下标，`NULL` → `-1`（忽略 ASCII 大小写） |
| `int strverscmp(const char *, const char *)` | string.h | `strverscmp(String, String)` | `int`（自然顺序版本比较） |
| `char *stpcpy(char *, const char *)` | string.h | `stpcpy(char[], String)` | 指针 → 已复制长度（`int`） |
| `char *stpncpy(char *, const char *, size_t)` | string.h | `stpncpy(char[], String, int)` | 指针 → `min(len, n)`；不足 `n` 以 char 0 填充 |

### CtypeFuncs —— `<ctype.h>`（16 个）

| C 接口 | Java 方法 | 返回类型映射 |
|---|---|---|
| `int isdigit(int)` | `isdigit(int)` | 0/非0 → `boolean` |
| `int isxdigit(int)` | `isxdigit(int)` | 0/非0 → `boolean` |
| `int isalpha(int)` | `isalpha(int)` | 0/非0 → `boolean` |
| `int isalnum(int)` | `isalnum(int)` | 0/非0 → `boolean` |
| `int isupper(int)` | `isupper(int)` | 0/非0 → `boolean` |
| `int islower(int)` | `islower(int)` | 0/非0 → `boolean` |
| `int isspace(int)` | `isspace(int)` | 0/非0 → `boolean` |
| `int isblank(int)` | `isblank(int)` | 0/非0 → `boolean` |
| `int isgraph(int)` | `isgraph(int)` | 0/非0 → `boolean` |
| `int isprint(int)` | `isprint(int)` | 0/非0 → `boolean` |
| `int ispunct(int)` | `ispunct(int)` | 0/非0 → `boolean` |
| `int iscntrl(int)` | `iscntrl(int)` | 0/非0 → `boolean` |
| `int isascii(int)` | `isascii(int)` | 0/非0 → `boolean` |
| `int tolower(int)` | `tolower(int)` | `int` |
| `int toupper(int)` | `toupper(int)` | `int` |
| `int toascii(int)` | `toascii(int)` | `int`（低 7 位掩码） |

### MiscFuncs —— `<strings.h>`（1 个）

| C 接口 | Java 方法 | 返回类型映射 |
|---|---|---|
| `int ffs(int)` | `ffs(int)` | `int`（1..32，0 表示无置位位） |

### StdlibFuncs —— `<stdlib.h>`（15 个）

| C 接口 | Java 方法 | 返回类型映射 |
|---|---|---|
| `int abs(int)` | `abs(int)` | `int` |
| `long labs(long)` | `labs(long)` | `long` |
| `long long llabs(long long)` | `llabs(long)` | `long long` → `long`（均 64 位） |
| `div_t div(int, int)` | `div(int, int)` | `div_t` → 嵌套类 `Div {int quot, rem}` |
| `ldiv_t ldiv(long, long)` | `ldiv(long, long)` | `ldiv_t` → 嵌套类 `Ldiv {long quot, rem}` |
| `lldiv_t lldiv(long long, long long)` | `lldiv(long, long)` | `long long` → `long`（复用 `ldiv`） |
| `int atoi(const char *)` | `atoi(String)` | `int` |
| `long atol(const char *)` | `atol(String)` | `long` |
| `long long atoll(const char *)` | `atoll(String)` | `long long` → `long`（复用 `atol`） |
| `double atof(const char *)` | `atof(String)` | `double`（委托 `strtod`） |
| `double strtod(const char *, char **)` | `strtod(String)` | `double`（省略 end 指针） |
| `long strtol(const char *, char **, int)` | `strtol(String, int)` | `long`（省略 end 指针） |
| `unsigned long strtoul(const char *, char **, int)` | `strtoul(String, int)` | 无符号 → `long` 位模式；溢出钳制 `ULONG_MAX`（`-1L`） |
| `unsigned long long strtoull(const char *, char **, int)` | `strtoull(String, int)` | 复用 `strtoul` |
| `void *bsearch(const void *, const void *, size_t, size_t, int (*)(const void *, const void *))` | `bsearch(T, T[], Comparator<? super T>)` | 指针 → 下标，未找到 → `-1`；比较函数 → `Comparator` |

### RegexFuncs —— `<fnmatch.h>`（1 个）

| C 接口 | Java 方法 | 返回类型映射 |
|---|---|---|
| `int fnmatch(const char *, const char *, int)` | `fnmatch(String, String, int)` | `0`/`FNM_NOMATCH` → `boolean`，`FNM_*` 常量定义于 `RegexFuncs` |

### MathFuncs —— `<math.h>`（50 个）

| C 接口 | Java 方法 | 返回类型映射 |
|---|---|---|
| `double sqrt(double)` | `sqrt(double)` | `double` |
| `double pow(double, double)` | `pow(double, double)` | `double` |
| `double floor(double)` | `floor(double)` | `double` |
| `double ceil(double)` | `ceil(double)` | `double` |
| `double fabs(double)` | `fabs(double)` | `double` |
| `double sin(double)` | `sin(double)` | `double` |
| `double cos(double)` | `cos(double)` | `double` |
| `double tan(double)` | `tan(double)` | `double` |
| `double asin(double)` | `asin(double)` | `double` |
| `double acos(double)` | `acos(double)` | `double` |
| `double atan(double)` | `atan(double)` | `double` |
| `double atan2(double, double)` | `atan2(double, double)` | `double` |
| `double exp(double)` | `exp(double)` | `double` |
| `double log(double)` | `log(double)` | `double` |
| `double log10(double)` | `log10(double)` | `double` |
| `double sinh(double)` | `sinh(double)` | `double` |
| `double log2(double)` | `log2(double)` | `double` |
| `double fmod(double, double)` | `fmod(double, double)` | `double`（Java `%` 即 C99 fmod） |
| `double remainder(double, double)` | `remainder(double, double)` | `double`（`Math.IEEEremainder`） |
| `double fmin(double, double)` | `fmin(double, double)` | `double` |
| `double fmax(double, double)` | `fmax(double, double)` | `double` |
| `double copysign(double, double)` | `copysign(double, double)` | `double` |
| `double round(double)` | `round(double)` | `double`（half-away-from-zero） |
| `double trunc(double)` | `trunc(double)` | `double` |
| `double cbrt(double)` | `cbrt(double)` | `double` |
| `double hypot(double, double)` | `hypot(double, double)` | `double` |
| `double expm1(double)` | `expm1(double)` | `double`（`Math.expm1`） |
| `double log1p(double)` | `log1p(double)` | `double`（`Math.log1p`） |
| `double exp2(double)` | `exp2(double)` | `double`（`pow(2.0, x)`） |
| `double rint(double)` | `rint(double)` | `double`（`Math.rint`，平局取偶） |
| `long lrint(double)` | `lrint(double)` | `double` → `long`（截断） |
| `long long llrint(double)` | `llrint(double)` | 复用 `lrint` |
| `long lround(double)` | `lround(double)` | `double` → `long`（half-away-from-zero） |
| `long long llround(double)` | `llround(double)` | 复用 `lround` |
| `double frexp(double, int *)` | `frexp(double)` | 出参 → 嵌套类 `Frexp {mantissa, exp}` |
| `double ldexp(double, int)` | `ldexp(double, int)` | `double`（`Math.scalb`） |
| `double modf(double, double *)` | `modf(double)` | 出参 → 嵌套类 `Modf {frac, integral}` |
| `double scalbn(double, int)` | `scalbn(double, int)` | `double`（`Math.scalb`） |
| `int ilogb(double)` | `ilogb(double)` | `int`；0→`INT_MIN`，NaN/inf→`INT_MAX` |
| `double logb(double)` | `logb(double)` | `double`；0→`-inf`，inf→`+inf` |
| `double nextafter(double, double)` | `nextafter(double, double)` | `double`（`Math.nextAfter`） |
| `double nextUp(double)` | `nextUp(double)` | `double`（`Math.nextUp`） |
| `double nextDown(double)` | `nextDown(double)` | `double`（`Math.nextDown`） |
| `double fdim(double, double)` | `fdim(double, double)` | `double`（正差） |
| `double fma(double, double, double)` | `fma(double, double, double)` | `double`（`Math.fma`，融合乘加） |
| `int isnan(double)` | `isnan(double)` | 0/非0 → `boolean` |
| `int isinf(double)` | `isinf(double)` | 0/非0 → `boolean` |
| `int isfinite(double)` | `isfinite(double)` | 0/非0 → `boolean` |
| `int isnormal(double)` | `isnormal(double)` | 0/非0 → `boolean` |
| `int signbit(double)` | `signbit(double)` | 0/非0 → `boolean`（符号位判断） |

### WcharFuncs —— `<wchar.h>`（15 个，对齐 musl `src/wchar/`）

| C 接口 | Java 方法 | 返回类型映射 |
|---|---|---|
| `size_t wcslen(const wchar_t *)` | `wcslen(String)` | `size_t` → `int` |
| `size_t wcsnlen(const wchar_t *, size_t)` | `wcsnlen(String, int)` | `size_t` → `int` |
| `int wcscmp(const wchar_t *, const wchar_t *)` | `wcscmp(String, String)` | `int` |
| `int wcsncmp(const wchar_t *, const wchar_t *, size_t)` | `wcsncmp(String, String, int)` | `int` |
| `int wmemcmp(const wchar_t *, const wchar_t *, size_t)` | `wmemcmp(char[], char[], int)` | `int` |
| `wchar_t *wcschr(const wchar_t *, wchar_t)` | `wcschr(String, int)` | 指针 → 下标，`NULL` → `-1`；`c==0` → `len` |
| `wchar_t *wcsrchr(const wchar_t *, wchar_t)` | `wcsrchr(String, int)` | 指针 → 下标，`NULL` → `-1`；`c==0` → `len` |
| `wchar_t *wcspbrk(const wchar_t *, const wchar_t *)` | `wcspbrk(String, String)` | 指针 → 下标，`NULL` → `-1` |
| `size_t wcsspn(const wchar_t *, const wchar_t *)` | `wcsspn(String, String)` | `size_t` → `int` |
| `size_t wcscspn(const wchar_t *, const wchar_t *)` | `wcscspn(String, String)` | `size_t` → `int` |
| `wchar_t *wcsstr(const wchar_t *, const wchar_t *)` | `wcsstr(String, String)` | 指针 → 下标，`NULL` → `-1` |
| `wchar_t *wmemchr(const wchar_t *, wchar_t, size_t)` | `wmemchr(char[], int, int)` | 指针 → 下标，`NULL` → `-1` |
| `wchar_t *wmemcpy(wchar_t *, const wchar_t *, size_t)` | `wmemcpy(char[], char[], int)` | `void*` 返回值省略 → `void` |
| `wchar_t *wmemmove(wchar_t *, const wchar_t *, size_t)` | `wmemmove(char[], char[], int)` | `void*` 返回值省略 → `void` |
| `wchar_t *wmemset(wchar_t *, wchar_t, size_t)` | `wmemset(char[], int, int)` | `void*` 返回值省略 → `void` |

## 关键语义差异

| 维度 | C（musl） | Java 版本 |
|---|---|---|
| 字符串结束 | NUL `'\0'` 结尾，`strlen` 按字节计数 | `String` 自带长度，`strlen` 按 UTF-16 码元计数（ASCII 下等价） |
| 字符类型 | `char`（8 位，`unsigned char` 参与比较） | `char`（UTF-16 码元），比较扩展到完整 Unicode |
| 裸内存 | `void *` 指向字节序列 | `byte[]`，比较用 `& 0xFF` 复刻无符号字节 |
| 指针返回 | 返回指向子串的 `char *` | 返回 `int` 下标，未找到 `-1` |
| `strrchr(s, 0)` | 连同结尾 NUL 一起查找，返回 NUL 位置 | 返回 `s.length()` |
| 分类函数 | 返回 0 / 非 0 的 `int` | 返回 `boolean` |
| 大小写折叠 | `strcasecmp`/`strncasecmp` 仅 ASCII（非 ICU） | 仅 ASCII |
| `abs(INT_MIN)` | 未定义行为 | 溢出回绕，仍为 `Integer.MIN_VALUE` |
| `atoi` 溢出 | 未定义行为 | 随 int 溢出回绕，与 musl「按负数累加」一致 |
| `atof` 解析 | 完整 `strtod`（含 C99 十六进制浮点 `0x.8p1`） | 委托 `strtod`，含 C99 十六进制浮点 |
| `strtod` 解析 | `__floatscan` 完整语义 | 十进制子串交 `Double.parseDouble`（正确舍入），十六进制用 `BigInteger` + `Math.scalb`；指数越界转 `±Inf`/`0` |
| `strtol` 溢出 | `ERANGE` 下返回 `LONG_MAX`/`LONG_MIN` | 正/负溢出钳制到 `Long.MAX_VALUE`/`Long.MIN_VALUE` |
| `strtoul/strtoull` 无符号 | `unsigned long`/`unsigned long long` 无符号回绕 | Java 以 `long` 表示 64 位位模式（超出 `LONG_MAX` 表现为负值）；溢出钳制到 `ULONG_MAX`（`-1L`），负号按无符号回绕 |
| `div/ldiv/lldiv` 返回值 | 返回 `div_t`/`ldiv_t`/`lldiv_t` 结构体 | 返回嵌套不可变类 `Div`/`Ldiv`；`long long` 复用 `long` |
| `bsearch` 查找 | 返回命中元素的指针，未找到 `NULL` | 返回下标，未找到 `-1`；`(base, nmemb, size)` → 泛型 `T[]`，比较函数 → `Comparator` |
| `memmem` 算法 | 长度 1..4 专用窗口 + Two-Way | 移植同一算法，返回下标（未找到 `-1`） |
| `memrchr/rawmemchr` 未找到 | `memrchr` 返回 `NULL`；`rawmemchr` 未找到为未定义行为 | 均返回 `-1`；`rawmemchr` 省略长度参数，查找整个数组 |
| `fnmatch` 类别 | `[[:alpha:]]` 依赖 locale，未完全支持 | 字符类 `[...]` 支持范围/取反/转义，但未实现 POSIX 字符类 `[[:alpha:]]` |
| `memcpy/memmove/memset` 返回值 | 返回目标指针（便于链式调用） | 省略 `void*` 返回值，方法返回 `void` |
| `round` 舍入 | C99 half-away-from-zero（负半值远离零） | `Math.round` 不符（负半值向 `+inf`）；以 `copySign(floor(|x|+0.5), x)` 复刻 |
| `fmod/remainder` | `fmod` 符号随被除数；`remainder` 为 IEEE 754 余数（商取最近整数、平局取偶） | Java `%` 即 `fmod`；`Math.IEEEremainder` 即 `remainder` |
| `fmin/fmax` NaN/±0 | 任一为 NaN 返回另一值；`fmin(+0,-0)=-0`、`fmax(-0,+0)=+0` | 需先判 NaN 再委托 `Math.min/max`（后者 NaN 语义不同） |
| `copysign` NaN 符号 | 位操作，符号取 NaN 载荷的符号位 | `Math.copySign` 对 NaN 符号约定为正 |
| `log2` 精度 | musl 精确舍入 | `Math.log(x)/Math.log(2)` 近似值 |
| `expm1/log1p` 精度 | musl 高精度实现 | 委托 `Math.expm1/Math.log1p` |
| `exp2` | 专用实现 | `pow(2.0, x)` |
| `lrint/lround` 超范围 | 未定义行为 | Java `(long)` 截断回绕 |
| `frexp/modf` 出参 | 通过指针出参返回 | 返回嵌套类 `Frexp`/`Modf` |
| `ilogb/logb` 特殊值 | `ilogb(0)=FP_ILOGB0`、`ilogb(NaN/inf)=FP_ILOGBNAN`；`logb(0)=-inf` | 0→`Integer.MIN_VALUE`，NaN/inf→`Integer.MAX_VALUE`；`logb` 用 -inf/NaN/+inf |
| `isnan/isinf/isfinite/isnormal/signbit` | 返回 0 / 非 0 的 `int` | 返回 `boolean`；`signbit` 以符号位判断 |
| `strverscmp` 排序 | 数字段按数值比较（自然排序） | 移植 musl `strverscmp` 算法 |
| `stpcpy/stpncpy` 返回 | 返回目标指针（指向结尾 NUL / `dst+n`） | 返回写入长度 `int`；`stpncpy` 不足 `n` 以 char 0 填充 |
| `wchar_t` 宽度 | Linux 上为 32 位码点 | Java `char` 为 16 位 UTF-16 码元，增补平面为代理对（语义差异） |
| `wmemchr/wmemcpy/wmemmove/wmemset` | 操作 `wchar_t *` | 操作 `char[]`（UTF-16 码元数组） |
| `ffs(i)` 语义 | 最低置位位编号（1..N），`i==0` → 0 | `Integer.numberOfTrailingZeros(i)+1`，`i==0` → 0 |

## 目录结构

目录组织仿 musl `src/` 按类型分子目录：主代码在 `src/ohos/musl/<type>/`，测试代码
独立放在 `test/ohos/musl/<type>/`，包名对应为 `ohos.musl.<type>`。

```
java_api/
├── README.md
├── src/ohos/musl/                  # 主代码（按类型子目录，仿 musl src/）
│   ├── string/StringFuncs.java     # ohos.musl.string  string.h / strings.h（25 个）
│   ├── ctype/CtypeFuncs.java       # ohos.musl.ctype   ctype.h（16 个）
│   ├── misc/MiscFuncs.java         # ohos.musl.misc    strings.h：ffs（1 个，对齐 musl src/misc/）
│   ├── stdlib/StdlibFuncs.java     # ohos.musl.stdlib  stdlib.h（15 个）
│   ├── regex/RegexFuncs.java       # ohos.musl.regex   fnmatch.h（1 个，对齐 musl src/regex/）
│   ├── math/MathFuncs.java         # ohos.musl.math    math.h（50 个）
│   ├── wchar/WcharFuncs.java       # ohos.musl.wchar   wchar.h（15 个，对齐 musl src/wchar/）
│   └── MuslDemo.java               # ohos.musl         演示入口
└── test/ohos/musl/                 # 测试（单独目录，按类型子目录）
    ├── Assert.java                 # ohos.musl         无依赖断言工具
    ├── TestRunner.java             # ohos.musl         测试套件入口
    ├── string/StringFuncsTest.java # ohos.musl.string
    ├── ctype/CtypeFuncsTest.java   # ohos.musl.ctype
    ├── misc/MiscFuncsTest.java     # ohos.musl.misc
    ├── stdlib/StdlibFuncsTest.java # ohos.musl.stdlib
    ├── regex/RegexFuncsTest.java   # ohos.musl.regex
    ├── math/MathFuncsTest.java     # ohos.musl.math
    └── wchar/WcharFuncsTest.java   # ohos.musl.wchar
```

## 编译与运行（独立于现有构建）

```sh
cd java_api
javac -d out $(find src test -name '*.java')
java -cp out ohos.musl.MuslDemo   # 演示
java -cp out ohos.musl.TestRunner # 测试（全部通过时退出码为 0）
```

## 测试

自带无依赖（不引入 JUnit）的测试套件，期望值均对照 musl 源码语义推演，覆盖正常值、
边界值与空串/前缀/越界/无符号字节/EOF/NaN/±inf 等情形：

```
== StringFuncsTest ==
  PASS
== CtypeFuncsTest ==
  PASS
== MiscFuncsTest ==
  PASS
== RegexFuncsTest ==
  PASS
== StdlibFuncsTest ==
  PASS
== MathFuncsTest ==
  PASS
== WcharFuncsTest ==
  PASS

ALL TESTS PASSED
```

## 后续接入建议（待想法验证通过后再评估）

当需要接入现有编译流程时，可考虑：

1. 在 `java_api/` 下补充 `BUILD.gn`，声明 Java 源集（`java_sources` / `java_library`）；
2. 或迁移到 OpenHarmony 的 ArkTS / NAPI 工程结构；
3. 接入前需与团队确认包名（`ohos.musl.string` / `ohos.musl.ctype` / `ohos.musl.stdlib` / `ohos.musl.math`）、目标语言（Java / ArkTS）与 ABI 约定。