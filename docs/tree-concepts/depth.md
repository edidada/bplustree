# 深度（Depth）

## 1. 定义

- **节点 $v$ 的深度** $\mathrm{depth}(v)$：从 **根** 到 $v$ 的 **最长（唯一）路径上的边数**。
- 根 $\mathrm{depth}(\mathrm{root})=0$；若 $u=\mathrm{pa}(v)$，则 $\mathrm{depth}(v)=\mathrm{depth}(u)+1$。
- 也可以说"深度 = 祖先个数（不含自己，根深度 0）"。

```
           (a) depth=0
          /   \
     (b) d=1  (c) d=1
      /            \
   (d) d=2         (e) d=2
                    最大深度 = 2
```

## 2. 深度与相关量的换算

- $\mathrm{depth}(v) + 1 = \ell(v)$（层次，见 [level.md](level.md)，默认 level 根=1）。
- 一棵 **树** 的"最大深度"数值上 = 树的高度 $h(\mathrm{root})$（见 [height.md](height.md)）。
- **树高 = 最大深度 ≠ 节点总数相关的必然**：n 个节点的树最坏深度 $n-1$（链），最好深度 $\lceil \log_2(n+1)\rceil-1$（二叉树满形）——BST 效率问题本质上就是"控制深度"问题。

## 3. 约定差异（最常见踩坑点）

| 约定 | depth(root) | 与 level 关系 | 采用方 |
|---|---|---|---|
| 本库默认（数边） | 0 | depth = level − 1 | CLRS、Knuth、大多数算法论文 |
| 国内部分教材（数点） | 1 | depth 与 level 同值 | 严蔚敏体系及大量本科讲义 |
| 部分实现 | 0 | — | 数组/递归天然 0 起 |

> 风险：AVL 平衡因子、红黑树黑高、堆高度公式若深度起算不同会整体差 1，**推导前先核对定义**。

## 4. 深度的关键性质

1. **祖先链长度**：$v$ 的严格祖先数 = $\mathrm{depth}(v)$。深度为 $d$ 的节点上方恰有 $d$ 条边。
2. **深度与递归栈**：DFS 递归深度 = 树高（最坏为 $n-1$）→ 深度决定迭代/递归转换的必要性（见 [traversal.md](traversal.md)）。
3. **对数界**：对"分支至少为 2"的树，若内部节点都有 $\ge 2$ 棵非空子树，则含 $n$ 个叶的树高 $\ge \lceil \log_2 n\rceil$；若每个内部节点恰 $\ge d$ 路（多路树），叶在深度 $k$ 至多 $d^k$ 个。
4. **深度互异/同深兄弟**：所有叶不必同深度；平衡树定义正是约束"叶深度之差"或"节点左右子树高度差"。

## 5. 各层级视角

- **本科**：会用递归算 depth（`depth(v) = 1 + max depth(children)`，注意叶返回 0），常与层次混淆。
- **硕士**：AVL/红黑树靠"限制高度差→限制深度→保证 $O(\log n)$ 查找"，深度被当作 I/O 或时间代价的等价物；B 树把深度压到 $O(\log_m n)$，磁盘场景每次深度 +1 就是一次随机 I/O（见 [b-tree.md](b-tree.md)）。
- **博士/研究**：随机树/随机 BST 的期望深度 $O(\log n)$；动态树的"深度均摊"（splay 的访问引理）等，见 [splay-tree.md](splay-tree.md)。

## 6. 求深度的实现要点

```c
/* 返回以 root 为根的子树的最大深度（空返回 -1，与空树高 -1 一致） */
int max_depth(struct node *root) {
    if (!root) return -1;
    int l = max_depth(root->left);
    int r = max_depth(root->right);
    return (l > r ? l : r) + 1;
}
```

## 相关概念

- 高度（方向相反、定义容易混）[height.md](height.md)
- 层次（与深度的 0/1 起算）[level.md](level.md)
- 平衡二叉树如何约束深度 [avl-tree.md](avl-tree.md)、[red-black-tree.md](red-black-tree.md)
- 深度 = 路径 I/O 代价 → [b-tree.md](b-tree.md)、[bplus-tree.md](bplus-tree.md)
