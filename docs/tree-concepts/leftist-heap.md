# 左偏堆 / 斜堆（Leftist Heap, Skew Heap —— 可并堆）

## 1. 动机：可并优先队列

普通二叉堆 [binary-heap.md](binary-heap.md) 合并两堆需 $O(n)$（重建）；若优先队列要频繁 **merge**（如 K 路合并、可并任务队列），需要**可并堆（mergeable heap）**。左偏堆是最简单的实现之一：**堆序 + 有意地让树"往左歪"**，使合并只沿右路径进行。

## 2. 定义：npl 与左偏性质

定义节点 $v$ 的 **null path length**：到最近空子节点的最短路径长度（空节点 npl=−1，叶 npl=0）。

**左偏（leftist）性质**：每个节点的
$$ \mathrm{npl}(v.\text{left}) \ge \mathrm{npl}(v.\text{right}) $$
即"左边离空更远"，右路径是树的最短路径。再配合堆序（最小堆：父 ≤ 子），得 **左偏堆（min-leftist heap）**。

推论：沿**右路径**走 $r$ 步后，子树至少含 $2^r-1$ 个节点 ⇒ 含 $n$ 个节点的左偏堆**右路径长 $O(\log n)$** —— 全部操作围绕右路径。

## 3. 合并（核心操作，递归一行核心思想）

```
merge(a, b):                      # 假设最小堆
    if a==null return b;  if b==null return a
    if a.key > b.key: swap(a,b)   # 保证 a 根更小
    a.right = merge(a.right, b)   # 递归把 b 并进 a 的右子树
    if npl(a.left) < npl(a.right): swap(a.left, a.right)  # 保持左偏
    a.npl = npl(a.right) + 1
    return a
```
- 代价沿右路径 $O(\log n)$；递归式极简（~6 行）。
- 其余操作都归约到 merge：
  - `insert` = merge(单点堆, 原堆)；
  - `delete-min` = merge(左子堆, 右子堆)；
  - `build`：把 n 个单点堆逐个 merge 是 $O(n\log n)$；用队列两两合并可得 $O(n)$。

## 4. 左偏 vs 二叉堆 vs 可并家族

| 堆 | merge | 取 min | 空间/实现 | 特性 |
|---|---|---|---|---|
| 二叉堆 | $O(n)$ | $O(1)$ | 数组、极简 | 无合并需求首选 |
| 左偏堆 | $O(\log n)$ | $O(1)$ | 指针、简单 | 教科书可并堆 |
| 斜堆 | $O(\log n)$ 摊还 | $O(1)$ | 同上但**无条件交换左右** | 更省"左偏约束"，摊还分析经典案例 |
| 二项堆 | $O(\log n)$ | $O(\log n)$ 查 | 树森林 | [binomial-heap.md](binomial-heap.md) |
| 斐波那契堆 | $O(1)$ 摊还 | $O(1)$ | 复杂 | [fibonacci-heap.md](fibonacci-heap.md) |

## 5. 各层级视角

- **本科**：左偏堆作为"可并堆"入门；考 merge 的递归模拟与 npl 计算。
- **硕士**：斜堆是"去掉左偏条件、合并后无条件交换子树"的左偏堆 → **摊还 $O(\log n)$**（势函数=右路径长度），展示"结构约束 vs 摊还分析"的取舍（与 splay 同思想 [splay-tree.md](splay-tree.md)）。
- **博士/应用**：左偏堆用于双端优先队列、外排序、以及可并任务调度；D 叉堆、配对堆（Pairing heap，摊还分析更微妙）是工程常用替代。

## 相关概念

- 优先队列基础 [binary-heap.md](binary-heap.md)
- 树形可并堆家族 [binomial-heap.md](binomial-heap.md)、[fibonacci-heap.md](fibonacci-heap.md)
- 有意"不平衡"的左偏 vs 退化树 [degenerate-tree.md](degenerate-tree.md)
