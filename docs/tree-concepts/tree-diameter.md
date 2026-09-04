# 树的直径（Tree Diameter，最长路径）

## 1. 定义

**直径（diameter）** = 树中**最长简单路径**的长度（按边数或权值和）。**直径端点**是这条路径的两端顶点。

- 树上任意两点路径**唯一**（[graph-tree.md](graph-tree.md) 性质 4），直径就是最长的"唯一路径"；
- 可能有多条直径，但**它们彼此共享中点**（无向树直径的交 = 一个点或一条边）——这个"中点唯一"性质是把直径当"树的中心线"用的基础。

## 2. 两遍 DFS/BFS 求直径（无负权/无权标准算法）

1. 任取一点 $s$，BFS/DFS 找离 $s$ 最远的点 $u$；
2. 从 $u$ 再走一次，离 $u$ 最远的 $v$ 即另一端点 → 路径 $u$–$v$ 为直径。

**正确性直觉**：树唯一路径下，任一点最远点必是某直径端点。若带负权边则此算法失效（须用树形 DP）。

## 3. 树形 DP 求直径（可处理带权，一次 DFS）

对每个节点维护"子树内从该点向下的最长链" $d1,d2$（两个最长的子链），
$$\text{直径} = \max_v (d1_v + d2_v)$$
（$d2$ 可空为 0 权）。一次后序 DP $O(n)$。

```c
/* 带权树直径：返回 diameter，post[v] 为子树内最远叶距离 */
void dfs(v, p) {
    long best = 0, second = 0;
    for (u in adj[v] if u != p) {
        dfs(u, v);
        long cand = post[u] + w(v,u);
        if (cand > best) second = best, best = cand;
        else if (cand > second) second = cand;
    }
    post[v] = best;
    ans = max(ans, best + second);   /* 过 v 的最长路径 */
}
```

## 4. 直径与其它概念的联系

| 概念 | 关系 |
|---|---|
| **深度/高度** [height.md](height.md) | 直径不一定经过根：$D=\max_u (h(u\text{.子1})+h(u\text{.子2})+2)$（二叉树版） |
| **树中心（centroid/center）** [tree-centroid.md](tree-centroid.md) | 直径中点是**center**（偏心距最小点，1–2 个），不是重心！两者不同 |
| 偏心距 ecc(v) | $=\max_u \mathrm{dist}(v,u)$；直径长 = $\max_v \mathrm{ecc}(v)$；半径 = $\min_v \mathrm{ecc}(v)=\lceil D/2\rceil$ |
| LCA/倍增 [lca.md](lca.md) | 动态加点求新直径：$\mathrm{dist}(u,v)=\mathrm{dep}(u){+}\mathrm{dep}(v){-}2\,\mathrm{dep}(\mathrm{lca})$；新点 $x$ 加入后新直径端点必在 {原两端点, x} 中任两者之一 → 用该技巧维护动态树的直径 |

## 5. 应用

- 消息传播/网络时延最长路径、二分答案判覆盖；
- 树上"选点最小化最大距离"类题都围绕直径/半径；
- 换根 DP 结合直径求"删某边后两棵树各自直径"等竞赛题。

## 6. 教学层级

- **本科竞赛/提高**：两遍 DFS、树形 DP；
- **硕士图算法**：直径与偏心距、在任意图上是 APSP 问题（树上是 $O(n)$ 特例）；
- **研究**：随机树的直径渐近（$\approx c\sqrt n$）、动态树直径。

## 相关概念

- 唯一路径性质 [graph-tree.md](graph-tree.md)
- 深度/高度公式 [height.md](height.md)、[depth.md](depth.md)
- 中心 vs 重心 [tree-centroid.md](tree-centroid.md)
- 路径距离计算 LCA [lca.md](lca.md)
