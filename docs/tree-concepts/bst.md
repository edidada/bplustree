# 二叉搜索树（Binary Search Tree, BST）

## 1. 定义与性质

**二叉搜索树**是满足下列**搜索树性质**的二叉树：对每个节点 $v$，设键值为 $k(v)$，则

$$ k(\text{左子树中任意节点}) < k(v) \le k(\text{右子树中任意节点}) $$
（允许重复时把相等放一侧；通常约定左小右大，键唯一）

```
           (50)
         /      \
      (30)      (70)
     /   \      /  \
  (20)  (40) (60) (90)
```
- 子树仍是 BST → **递归结构**。
- **中序遍历 BST 得到严格递增序列**（[traversal.md](traversal.md)），这是 BST 一切操作正确性的检验器，也是"中序+前后序还原/验证"的钥匙。

## 2. 操作与复杂度

| 操作 | 做法 | 最坏 | 平均/随机 |
|---|---|---|---|
| 查找 find | 从根二分下走 | $O(h)=O(n)$ | $O(\log n)$ |
| 插入 insert | 查找失败处挂新叶 | $O(h)$ | $O(\log n)$ |
| 删除 delete | 叶：直接删；单子：托孤；双子：用**中序后继/前驱**顶替再删 | $O(h)$ | $O(\log n)$ |
| 前驱/后继 | 树内中序相邻 | $O(h)$ | $O(\log n)$ |
| 第 k 小/排名 | 需子树 size 字段（order-statistic tree） | $O(h)$ | $O(\log n)$ |

其中 $h$ 为树高。**最坏** $h$ 可达 $n-1$（有序插入 → [退化树](degenerate-tree.md)），因此 BST 裸用最坏 $O(n)$。

## 3. 三个重要证明/数学结论

1. **期望高度**：$n$ 个不同键以随机顺序插入得到的 BST，期望高度 $O(\log n)$（精确渐近 $2e\ln n \approx 1.39\log_2 n$）—— 随机化 treap 就是"显式模拟这个随机过程"（[treap.md](treap.md)）。
2. **二叉搜索树与比较排序**：n 个键的 BST 决策树模型给出下界 $\Omega(n\log n)$；用 BST 排序即"插入后中序输出"，平均 $O(n\log n)$。
3. **平衡的必要性**：任何保证"树高 $O(\log n)$"的 BST（AVL/红黑/splay 摊还）都只是**控制高度**的手段，查找逻辑与裸 BST 完全相同 → 后续所有平衡树都可看成"BST + 平衡维护协议"。

## 4. 关联扩展结构（硕士级一览）

| 变体 | 加的信息 / 规则 | 文件 |
|---|---|---|
| AVL | 左右子树高度差 ≤ 1 | [avl-tree.md](avl-tree.md) |
| 红黑树 | 黑高平衡 + 着色规则 | [red-black-tree.md](red-black-tree.md) |
| Splay | 访问即旋转到根（摊还） | [splay-tree.md](splay-tree.md) |
| Treap | 随机堆序 + 旋转 | [treap.md](treap.md) |
| 替罪羊树 | 局部重建 | [scapegoat-tree.md](scapegoat-tree.md) |
| B 树/B+ | 多路、节点为块 | [b-tree.md](b-tree.md) |
| 序统计 | 每节点记子树 size | —（在 AVL 中常见） |

## 5. 考点与应用

- **前驱/后继、删除的三种情况**是最常考的推导题；
- **验证一棵树是否为 BST**：中序是否有序；或用递归区间法（每个节点值落在 (min,max)）；
- BST 顺序性质使"区间查询（找 [l,r] 所有键）"可在 $O(h+\text{输出数})$ 完成——这是 B+ 树范围查询在内存版的原型（[bplus-tree.md](bplus-tree.md) 对比区）。

## 相关概念

- 中序有序性 [traversal.md](traversal.md)
- 退化的根源 [degenerate-tree.md](degenerate-tree.md)
- 平衡家族 AVL/红黑/B 树 [avl-tree.md](avl-tree.md)、[b-tree.md](b-tree.md)
- 随机 BST ↔ Treap [treap.md](treap.md)
