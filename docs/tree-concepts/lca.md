# 最近公共祖先（Lowest Common Ancestor, LCA）

## 1. 定义

有根树中两节点 $u,v$ 的 **最近公共祖先 LCA(u,v)** = 同时是 $u,v$ 祖先的**深度最大**节点。

- 显然根是所有点对的 LCA 之一；LCA(u,u)=u；
- LCA 是"树上距离"与"子树区间"等一切路径查询的基础：
$$\mathrm{dist}(u,v) = \mathrm{depth}(u) + \mathrm{depth}(v) - 2\cdot\mathrm{depth}(\mathrm{LCA}(u,v))$$
（深度按边计 [depth.md](depth.md)）。

## 2. 五种经典求法（按复杂度/用途）

| 方法 | 预处理 | 单次查询 | 特点 |
|---|---|---|---|
| 朴素爬升 | $O(n)$ | $O(n)$ | 深的先上移再同步 |
| **倍增（binary lifting）** | $O(n\log n)$ | $O(\log n)$ | 最普及：`up[v][k]`=v 的 $2^k$ 级祖先 |
| **欧拉序 + RMQ** | $O(n\log n)$（ST 表） | $O(1)$ | LCA = 欧拉序区间内 depth 最小点（RMQ） |
| **Tarjan 离线** | $O(n\alpha(n))$ | 离线批量 | DFS + 并查集，一次处理全部询问 |
| **树链剖分（HLD）** | $O(n)$（剖分） | $O(\log n)$ | 附带重链信息，配合线段树做链上修改/查询 |

```c
/* 倍增 LCA */
int lca(int u, int v) {
    if (dep[u] < dep[v]) swap(u, v);
    for (int k = LOG-1; k >= 0; k--)          /* 1) u 提到与 v 同深 */
        if (dep[u] - (1<<k) >= dep[v]) u = up[u][k];
    if (u == v) return u;
    for (int k = LOG-1; k >= 0; k--)          /* 2) 一起倍增跳（不到父） */
        if (up[u][k] != up[v][k]) { u = up[u][k]; v = up[v][k]; }
    return up[u][0];
}
```

## 3. 各方法深入

- **倍增**：`up[u][0]=父`，`up[u][k]=up[up[u][k-1]][k-1]`；查询两段降幂。还能顺带维护"第 k 级祖先"与**路径上 min/max/第 k 大**（倍增 RMQ）——最常用。
- **欧拉序 + RMQ**：DFS 进出各记录一次得到长度 $2n{-}1$ 的欧拉序列，LCA(u,v) = 序列中 u、v 首次出现之间的**深度最小节点**；RMQ 用稀疏表 $O(1)$ → 总 $O(1)$ 查询，配合 [cartesian-tree.md](cartesian-tree.md) 还能把 RMQ 预处到 $O(n)$。
- **Tarjan 离线**：DFS 时"已回溯完"的点并入其父的并查集，处理以当前点为端点的询问 → 均摊近乎线性，适合超大规模离线。
- **树链剖分（HLD）**：按"子树最大儿子"剖重链；把树映射成若干连续区间（dfs 序）后，路径查询 = $O(\log n)$ 段区间问题 → 配合 [segment-tree.md](segment-tree.md)/[fenwick-tree.md](fenwick-tree.md) 支持**链上加、链上求和**（树剖是"把树上路径摊平成区间"的通用框架）。

## 4. LCA 上的区间化副产品（必提）

- **dfs 序（时间戳）**：进入/离开子树给节点编号 → 每个节点子树 = **一段连续区间** → 子树加/子树和用 BIT/线段树解决；
- 这就是 [segment-tree.md](segment-tree.md)、[fenwick-tree.md](fenwick-tree.md) 与树结合的最常见入口；
- 虚树（virtual tree）：仅保留关键点 + 它们两两 LCA，把 $O(k)$ 个点的树问题压缩，用于多询问（关键点总数受限）场景。

## 5. 树上路径经典题（LCA 应用清单）

- 树上距离 / 两点路径边权统计；
- 路径加 + 单点查（树上差分）；
- 点/边权第 k 大路径查询（倍增或树剖套主席树）；
- 直径动态维护用 LCA 距离公式（[tree-diameter.md](tree-diameter.md)）；
- 换根 LCA：固定根 r 时 `LCA_r(u,v)` 可由三个固定根 LCA 组合得到。

## 6. 教学层级

- **本科/竞赛入门**：倍增；
- **硕士算法**：RMQ 转化、Tarjan 离线、树剖/虚树；
- **研究/高级**：动态树（Link-Cut Tree）把 LCA 问题扩展到**边动态增删**的树上（$O(\log n)$ 摊还，以 splay 为基础，[splay-tree.md](splay-tree.md) 已铺垫其引理）。

## 相关概念

- 深度/祖先定义 [depth.md](depth.md)、[tree-terminology.md](tree-terminology.md)
- 倍增跳转的存储 [tree-representation.md](tree-representation.md)
- 区间化工具 [segment-tree.md](segment-tree.md)、[fenwick-tree.md](fenwick-tree.md)
- RMQ↔LCA 的桥梁 [cartesian-tree.md](cartesian-tree.md)
