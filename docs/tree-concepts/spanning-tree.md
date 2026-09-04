# 生成树与最小生成树（Spanning Tree, MST）

## 1. 定义

- **生成树（spanning tree）**：连通图 $G=(V,E)$ 的生成树是包含 $G$ **全部 $n$ 个顶点**且自身是树（连通、$n-1$ 条边、无环）的子图；
- **生成森林**：对不连通图，每个连通分量一棵生成树；
- **最小生成树（MST）**：**加权**连通图中，总边权最小的生成树。
- **次小生成树 / 瓶颈生成树** 等是其扩展（瓶颈 MST：最大边权最小）。

```
G                         一棵生成树
a──3──b                  a──3──b
│     │                  │
4     2                  4
│     │                  │
c──1──d                  c      d?（图省略示意，删边后仍连通且 n-1 边）
```
生成树总数可指数多（完全图 $n^{n-2}$，Cayley，[prufer-sequence.md](prufer-sequence.md)）→ 直接枚举不可行。

## 2. MST 的割性质与圈性质（证明一切贪心的钥匙）

- **割性质（cut property）**：对任意割 $(S,V\setminus S)$，**跨割的最小权边必属于某棵 MST**；
- **圈性质（cycle property）**：对任意圈，**圈上的最大权边必不属于任何 MST**（严格最大时）。

这两个"局部最优=全局可行"的交换论证，是 Kruskal/Prim 正确性的共同基础（硕士证明必考）。

## 3. 经典算法

### Kruskal（边视角 + 并查集）
1. 边按权升序；
2. 逐条加边，若两端已连通（并查集 find 同根）则跳过，否则加入并 union；
3. 到 $n-1$ 条边为止。
复杂度 $O(m\log m)$（排序主导）。正确性：选最小边且不成环 ⇔ 每条都在某割的最小边位置上。

### Prim（点视角 + 优先队列/Dijkstra 式）
1. 从任一点出发维护"已加入集合"；
2. 每次用最小堆取"跨出集合的最小边"加入，更新邻居；
3. 二叉堆 $O(m\log n)$；斐波那契堆 $O(m+n\log n)$（[fibonacci-heap.md](fibonacci-heap.md)）。
密集图 Prim（邻接矩阵 $O(n^2)$）常优于 Kruskal。

### Borůvka
每轮各连通分量选最小出边合并：$O(m\log n)$，适合并行/分布式（历史最古老的 MST 算法）。

## 4. 与树概念的交汇

- 判环=并查集用树"连通+无环"的等价（[union-find.md](union-find.md)）；
- 每加一条 MST 边都是一次"树并树"（[forest.md](forest.md)）；
- **MST 是"删掉图的多余边得最省树"**：树的 $m=n-1$ 极小性（[graph-tree.md](graph-tree.md)）决定了 MST 一定存在（图连通时）。

## 5. 应用与变体

- 应用：网络布线最小成本、聚类（单链接聚类即 Kruskal 顺序）、近似（旅行商 MST 下界 2-近似、Steiner 近似）；
- 变体：最小瓶颈树、度数限制 MST、动态 MST、欧几里得 MST（Delaunay 三角化子图）。

## 6. 教材位置

- **本科**：Kruskal/Prim 手算、与 Dijkstra 的类比；
- **硕士算法（CLRS 第 23 章）**：割/圈性质证明、堆优化；
- **研究**：随机化/并行 MST、动态图。

## 相关概念

- 判环与并查集 [union-find.md](union-find.md)
- 树的充要条件 [graph-tree.md](graph-tree.md)
- 生成树计数 Cayley [prufer-sequence.md](prufer-sequence.md)
- 堆优化 [binary-heap.md](binary-heap.md)、[fibonacci-heap.md](fibonacci-heap.md)
