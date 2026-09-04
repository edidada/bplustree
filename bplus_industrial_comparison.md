# 本仓库 B+ 树实现 vs 工业级 B+ 树：功能与性能对比

- 日期：2026-09-04
- 本仓库版本：commit `93cee93`（disk-io 分支），实现位于 `lib/bplustree.c`（约 1200 行，单文件）
- 工业参照实现：
  - **LMDB**（Symas Lightning Memory-Mapped Database）—— 嵌入式、mmap 的磁盘 B+ 树键值库
  - **SQLite btree**（v3.45.1）—— 本机可跑，直接同负载实测对照
  - **InnoDB** —— 仅作架构级提及（MySQL 存储引擎中的聚簇索引 B+ 树）

> 说明：以下「本仓库」的描述均对照 `lib/bplustree.c` 源码逐条核实；LMDB/SQLite/InnoDB 的描述来自其官方文档与公开资料，不构成代码级审计。

---

## 1. 功能 / 架构对比矩阵

| 维度 | 本仓库（bplustree） | LMDB | SQLite btree | InnoDB |
|---|---|---|---|---|
| 索引结构 | 经典磁盘 B+ 树（内节点存 key+child 偏移，叶节点存 key+value，叶间有 prev/next 兄弟链） | B+ 树 + Copy-on-Write | B+ 树变体（sqlite3 btree.c：内部页 key+child，叶 cell 存 payload，大值 overflow 页链） | B+ 树聚簇索引（数据按主键有序存放于叶） |
| 存储单元 | 自定义 block（2^n 字节，需为 2 的幂，如 512/4096），每个节点 = 1 个 block | 页（默认系统页大小，可配 512B~64KB），整页 COW | 页（默认 4096B，512B~64KB） | 页（默认 16KB） |
| 数据模型 | 仅 key→long 整数；无类型系统 | key/value 二进制（上限由页决定，key 约 <1/2 页） | SQL 之上的任意行/列；纯 btree 层为 key→record | SQL 行/列 |
| key 类型 | `int`（32 位有符号） | 任意字节串，页内有序 | 排序键（可任意类型编码） | 任意主键 |
| value 存储 | 固定 `long`（8B），value==0 表示删除 | 变长 | 变长 + 溢出页 | 变长 |
| 内存缓存 | 仅 **5 个 block**（`MIN_CACHE_NUM=5`）进程内节点缓冲（读写路径暂存）；其余每次访问直接 `read()`/`pwrite()`，依赖 OS page cache | 无用户缓存：整库 mmap 只读映射，页即内存，读零拷贝 | 自管理页缓存（`sqlite3_pagecache`，可配大页）；同样受益于 OS cache | 大块 Buffer Pool（默认 128MB 起） |
| 查找 | 根到叶每层二分（`key_binary_search`） | mmap 直接页内查找 + B+ 树游标 | 页内二分 + cell 解析，VDBE 驱动 | Buffer Pool 内 B+ 树 |
| 范围查询 | `bplus_tree_get_range(k1,k2)`：seek 后沿叶兄弟链右移；**仅返回遍历中最后遇到的 data 值**（≈范围内最右键的值，空区间返回 -1，无法取回集合/计数） | `MDB_cursor` 游标 + `MDB_GET_RANGE`，可逐条迭代 | SQL `WHERE k BETWEEN` / `ORDER BY` 游标，可迭代 | 游标 + 区间扫描 |
| 删除 | 无独立 API：`put(key, 0)` 即删；underflow 会借/合并兄弟，空间进 `free_blocks` 复用 | 事务内 delete，COW 生成新版本，free 页进 freelist | SQL DELETE，btree 层删除 cell，页合并 | DELETE + 二级索引维护 |
| 持久化语义 | **非事务**：每次改动直接写回磁盘文件（经 OS 页缓存），仅在 close/deinit 时 `fsync`；崩溃可能留下不一致状态 | **ACID 事务**：COW + 双 meta 页 + txn id 原子切换，崩溃恢复内建；无 WAL、无日志 | **ACID 事务**（默认 rollback journal，可 WAL）；日志 + 原子提交 + 页级恢复 | **ACID**：redo/undo log，崩溃恢复（recovery） |
| 元数据/一致性 | `.boot` 文件明文存 root/block_size/file_size/free_blocks（16 位 hex）；**无校验和、无 torn-write 防护、无损坏检测** | 双 meta 页互校验 + txn id | 页头 + journal/WAL 校验 | checksum + doublewrite buffer 等 |
| 事务 | 无（每 op 独立立即写） | 写事务（单写者）、只读事务（多读，MVCC 快照） | 读写事务（库级锁，可 WAL 多读一写） | 行级锁 + MVCC |
| 并发 | **无锁、单线程**；库内多处共享全局状态（`_block_size`/`_max_entries` 为 `static`），进程内同时只宜存在一种 block 配置 | 单写多读、跨进程共享（只读者无锁）；工业级并发 | 同进程多线程读写安全；跨进程由文件锁 | 服务端级高并发 |
| 多实例 | 受全局静态量限制，非纯粹实例化设计 | 支持多 environment/多库（子库） | 可同时打开多库/多连接 | 多表空间 |
| 文件格式 | 自研：数据文件 + `.boot` 文本元数据；**block 配置不可变**（改 block_size 打开旧文件是未定义行为，需删除重建） | 单文件 + 可选锁文件；COW 使文件格式对崩溃稳健 | 单文件（+WAL/SHM）+ 页校验 | 表空间 + 日志组文件 |
| 崩溃/断电 | 无保证；数据文件可能半写，需外部重建成文件 | 掉电安全（meta 页原子切换） | 掉电安全（journal/WAL） | 掉电安全（redo log） |
| 备份/工具 | 无（copy 文件即可，但须先 close） | `mdb_copy`（热备，COW 特性） | `sqlite3` CLI / backup API / VACUUM | mysqldump/物理备份 |
| 调试 | `bplus_tree_dump()` 打印树形；`_BPLUS_TREE_DEBUG` | 无内置 | EXPLAIN/PRAGMA 丰富 | performance_schema 等 |
| 定位 | 学习/教学级内核，API 极简（5 个函数） | 嵌入式 KV 库（OpenLDAP 等使用） | 世界最广泛部署的嵌入式数据库 | 服务器级 OLTP 引擎 |

### 差异小结（结构性）

1. **数据模型**：本仓库只有 `int key → long data`，且 `0` 是删除哨兵、`-1` 表示"不存在"，用户数据范围受限；工业实现提供二进制 key/value 或完整 SQL 层。
2. **持久化一致性是最大差距**：本仓库无事务/日志/校验和，crash 后索引文件可能损坏；LMDB/SQLite/InnoDB 均保证进程崩溃/掉电一致性。
3. **缓存策略极端不同**：本仓库刻意只保留 5 个节点在用户态，页面访问退化为 `read()/pwrite()` 系统调用（好处：简单、内存占用恒定 ~5×block，坏处：无用户态页缓存、每次读路径系统调用开销）；LMDB 干脆全库 mmap 零拷贝；SQLite/InnoDB 维护大块缓存池。
4. **范围查询语义不完整**：`get_range` 只回传遍历到的最右一个 data，无法取回区间元素/计数，与工业实现的游标迭代差距明显。
5. **并发与多实例**：无锁单线程 + 全局静态配置，仅适合单进程单树场景。

---

## 2. 同机实测（性能对比）

### 2.1 环境与方法

- CPU：AMD Ryzen 7 8845H；内存 13 GB；数据放本机磁盘（`/tmp`，ext4）。
- 负载统一为（键均取 1..1,000,000，value==key，命中率 100%）：
  1. **顺序插入 1,000,000** 条
  2. **随机点查 1,000,000** 次（PK 命中）
  3. **范围查询 250,000** 次（窗口宽度 ≤512）
- 对照双方都是 C 程序、相同调用风格：
  - 本仓库：直接调 `bplus_tree_init/put/get/get_range`，block=4096（页面含 338 个 key，树的逻辑层级 2~3 层）。
  - SQLite：C API，`INTEGER PRIMARY KEY` 表（rowid 即聚簇 B 树），单事务插入；生产推荐配置 `journal_mode=WAL; synchronous=NORMAL`。
- 所有测量在 OS page cache 热态下进行（数据量 ~几十 MB << 内存），反映**热缓存下的引擎自身开销**，不反映冷盘 IO。

### 2.2 结果

| 负载 | 本仓库 bplustree | SQLite 3.45.1 btree | 相对（SQLite/本仓库） |
|---|---|---|---|
| 顺序插入（1M） | 2.28 s → **438k keys/s** | 0.35 s → **2,861k keys/s** | SQLite 快 **6.5×** |
| 随机点查（1M） | 2.12 s → **472k keys/s** | 3.28 s → **305k keys/s** | 本仓库快 **1.55×** |
| 范围查询（250k，宽≤512） | 0.85 s → **293k q/s** | 2.47 s → **101k q/s** | 本仓库快 **2.9×** |

另：本仓库既有随机回归用例（`tests` 3,000,000 行混合 `i/r/s` 操作，block=512）在相同机器上约 **8.8~9.0 s ≈ 340k ops/s**，与该处小页场景一致。

### 2.3 结果解读

- **插入慢 6.5× 的原因**：SQLite 把 1M 次插入放在一个事务里，页缓存放行写入、仅在 COMMIT 时 fsync；本仓库每次 `put` 都是完整"读/改写路径 + 直接写盘"，即使没有每次 fsync，也要付出每次操作多块读写的代价，且用户态节点缓存仅 5 块，读路径普遍落系统调用。**若本仓库也做批量/缓冲 flush，差距会显著缩小**，但当前 API 设计没有该选项。
- **点查与范围反而更快**：负载全命中且热缓存，本仓库每层只需一次 `read()` + 页内二分（约 2 层/次），无 SQL 层（prepare/VDBE/绑定解析）与事务记账开销，也没有 cell/record 的变长解析。这印证了"**功能与一致性保障越少，路径越短**"——在纯读、数据热、单线程场景下微型实现可以胜出。
- **范围查询差距大**：SQLite 每次 `COUNT BETWEEN` 都要过完整 SQL/btree 游标路径；本仓库只是叶链顺序扫。若用 SQLite 游标连续迭代替代逐条 SQL，SQLite 的范围遍历吞吐会更高——该对照偏向"逐请求语义"，对工业库不构成上限结论。

### 2.4 局限与公平性说明

1. 全部热缓存：不含磁盘随机 IO、不含冷启动；真实磁盘负载下缓存与 fsync 策略将主导差异。
2. 语义不对等：SQLite 每事务 durable（WAL+NORMAL 一次 fsync），本仓库非事务、仅 close fsync。本仓库"快"的读侧数据**不保证掉电安全**。
3. 未测并发（本仓库不支持并发，无可比性）、未测大 value、未测 10^8 以上规模与磁盘空间耗尽、未测崩溃注入。
4. 一个 python 绑定的 SQLite 对照（每 op 含解释器开销）点查仅 ~193k/s、范围 ~54k/s，明显低于 C 同层结果——**绑定层开销可以掩盖引擎真实能力**，因此上表统一用 C 对 C。
5. 单次测量，无多次取中位数；仅供量级参考。

### 2.5 工业实现公开参考（非本机，引自官方/社区资料）

- LMDB 官方定位：只读零拷贝（mmap）、多读者无锁并发、COW 写事务、免 WAL 的 ACID（见 lmdb.tech/doc）。其官方 microbenchmark 在同机对比中，写/读吞吐均以显著幅度领先 SQLite 与 BerkeleyDB（"读写吞吐均优于"见 readthedocs 概述）。建议以官方 benchmark 页数据为准，避免复述不可考数字。
- SQLite 官方自述侧重：正确性/兼容性/低内存占用优先于极致吞吐；btree 层单点查询微秒级是常态（与上表同量级）。
- 结论性参考：工业级实现的优势不在"单线程热读"，而在 **事务、崩溃一致、并发、可变长数据与运维能力**。

---

## 3. 结论与适用场景

| | 本仓库 bplustree 适用 | 工业实现适用 |
|---|---|---|
| 教学/内核理解 | ★★★★★ 单文件、结构清晰、可断点调试 | ★ 代码量大、概念多 |
| 百万级热读/顺序范围读 | ★★★★（无锁、路径短） | ★★★★（并发/页缓存更稳） |
| 大批量写入 | ★★（无批量事务、逐 op 写） | ★★★★★（事务/缓存批量落盘） |
| 崩溃恢复/掉电安全 | ✗ 不保证 | ★★★★★ |
| 并发读写 | ✗ 不支持 | ★★★★★ |
| 大 value/任意 key | ✗ int key + long value | ★★★★★ |
| 生产长期运维 | ✗ 无工具/校验 | ★★★★★ |

一句话总结：**这是一颗结构标准、可读性极佳的教学级磁盘 B+ 树内核；在单线程、热缓存、整数键值、范围读为主的场景下能与工业实现掰手腕（点查/范围快 1.5~3×），但在事务原子性、崩溃一致性、并发与数据模型四个维度与 LMDB/SQLite/InnoDB 存在代际差距**，不宜直接用于需要持久化可靠性的生产数据。

---

## 4. 复现方法

同机实测所用两个 bench 已入库（`bench/` 目录，同一负载、可直接对比），复现命令：

```bash
# 本仓库（自 lib/bplustree.c 编译）
gcc -O2 -Wall bench/bplus_bench.c lib/bplustree.c -Ilib -o bplus_bench
./bplus_bench

# SQLite（C API，需 libsqlite3-dev）
gcc -O2 -Wall bench/sqlite_bench.c -lsqlite3 -o sqlite_bench
./sqlite_bench

# 本仓库 300 万随机操作回归（仓库自带，见 tests/）
cd build && python3 ../tests/testcase_generator.py && ctest -C Release --output-on-failure
```

仓库内自动测试（真实回放 300 万随机操作 + 文件驱动 range/in 查询）见 `.github/workflows/cmake-single-platform.yml` 与 `tests/`。
