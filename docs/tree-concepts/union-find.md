# 并查集（Union-Find / Disjoint Set Union, DSU）

## 1. 定义与树形本质

**并查集**维护一个由若干**互不相交集合**组成的**森林**，支持：

- `find(x)`：返回 $x$ 所在集合的代表元；
- `union(x, y)`：合并 $x$、$y$ 所在集合。

森林表示：每棵树 = 一个集合，树根 = 代表元，每个节点只存 `parent`（**双亲表示法**，见 [tree-representation.md](tree-representation.md)）：

```
集合 A = {1,2,4}        集合 B = {3,5}
       1                     3
      / \                     \
     2   4                     5
fa[2]=fa[4]=1   fa[5]=3
find(4) = 1（沿父上溯到根）    union: 把一棵树的根挂到另一棵树根下
```

## 2. 两个优化（缺一不可的分析核心）

1. **按秩合并（union by rank/size）**：总是把小树根挂到大树根下，控制树高 ≤ $\log n$。
   - 秩可定义为高度上界或子树大小；只影响 `find` 的深度。
2. **路径压缩（path compression）**：`find` 时把沿途节点**直接挂到根**下，树被压平。
   - 可在递归/迭代 find 中实现；只读性质的改变，不破坏集合归属。

## 3. 复杂度定理（硕士必知结论）

- 只按秩合并：每操作 $O(\log n)$；
- 只路径压缩（无按秩）：$O(\log n)$ 摊还（边界情形稍复杂）；
- **两者都做**：单次操作摊还 $O(\alpha(n))$，其中 $\alpha$ 是 **反 Ackermann 函数**，对任何现实规模 $\alpha(n)\le 4$（视为常数）。这由 Tarjan 的**摊还分析**给出（势能按"秩级别"分桶），是"几乎常数"复杂度的教科书来源。

## 4. 为什么它是"树概念"集合

- 树高被"按秩 + 压缩"控制在 $O(\log n)/O(\alpha(n))$——与平衡 BST 控高思想相同，只是目标**只是代表元**、不要求有序；
- **森林结构动态改变**：合并会改变树的形态（加边），但保持每棵"树"的**无环性**；
- 有趣性质：每次 union 是在**最小生成树 Kruskal** 里加边前判环的基础（[spanning-tree.md](spanning-tree.md)）——"若两端已在同一棵树里，加边必成环"利用了树"无环+连通"的等价性质 4（[tree.md](tree.md)）。

## 5. 扩展变体（竞赛/工程）

- 带权并查集：维护每个节点到根的权（距离/异或和）做相对关系判断；
- 按 size 合并 + 可撤销（栈记录历史）：离线回滚；
- 二分图判定、离线 LCA（Tarjan 离线算法）皆用 DSU。

## 6. 复杂度公式速查

| 方案 | 每次 find/union |
|---|---|
| 朴素（无优化） | $O(n)$ 最坏 |
| 按秩合并 only | $O(\log n)$ |
| 按秩 + 路径压缩 | $O(\alpha(n))$ 摊还 |

## 相关概念

- 树 = 集合、森林递归定义 [forest.md](forest.md)、[tree.md](tree.md)
- 双亲表示 [tree-representation.md](tree-representation.md)
- 树高控制思想 [avl-tree.md](avl-tree.md)、[b-tree.md](b-tree.md)
- 判环用树性质 → Kruskal MST [spanning-tree.md](spanning-tree.md)
