# 树的遍历（Traversal：先序 / 中序 / 后序 / 层序）

## 1. 三类基本遍历（DFS 家族）

对二叉树定义：

- **先序 / 前序（preorder, DLR）**：根 → 左子树 → 右子树
- **中序（inorder, LDR）**：左子树 → 根 → 右子树
- **后序（postorder, LRD）**：左子树 → 右子树 → 根

```
        (1)
       /   \
    (2)     (3)
   /  \       \
 (4)  (5)    (6)

先序: 1 2 4 5 3 6
中序: 4 2 5 1 3 6
后序: 4 5 2 6 3 1
```

三者的递归实现只是把"访问根"移到递归左右子树之前/中/后。

## 2. 层序遍历（BFS 家族）

**层序（level-order）**：用队列，按层次从上到下、同层从左到右：
```
1 → 2 3 → 4 5 6
```
- 复杂度 $O(n)$，空间 $O(w)$，$w$ 为该层最大宽度（完全二叉树 $w=\lceil n/2\rceil$）。
- 是求层数、构造完全树、按层打印、二叉堆判空槽的工具。

## 3. 遍历的经典结论（全部必考）

1. **唯一性**：先序 + 中序（或后序 + 中序）可**唯一确定**一棵二叉树；先序 + 后序不唯一。
2. **BST 性质**：对二叉搜索树，中序输出 = **有序递增序列**（[bst.md](bst.md) 的钥匙）。
3. **递归深度**：先/中/后序遍历递归栈深 = 树高（退化树最坏 $n$）。
4. **一般树/森林的遍历**：
   - 树的先序 = 先根：访问根，再依次先序遍历每棵子树；
   - 树的后序 = 后根：先依次后序遍历子树，再访问根；
   - 森林遍历 = 依次遍历各棵树（先序 = 依次先序）。
5. **LCRS 对应**（高频考点）：一般树先序 = LCRS 二叉树先序；一般树后序 = LCRS 二叉树**中序**（见 [ordered-tree.md](ordered-tree.md)、[tree-representation.md](tree-representation.md)）。
6. **表达式树**：中序 = 中缀式（去括号版），后序 = 后缀式（逆波兰），先序 = 前缀式（[expression-tree.md](expression-tree.md)）。

## 4. 非递归（显式栈）实现思想

统一套路（以中序为例）：栈存"待处理"节点：

```c
void inorder_iter(struct node *root) {
    struct node *st[1024]; int top = 0;   /* 示意：工程用动态栈 */
    struct node *cur = root;
    while (cur || top) {
        while (cur) { st[top++] = cur; cur = cur->left; }   /* 沿左走到底 */
        cur = st[--top];
        visit(cur);                                          /* 出栈访问 */
        cur = cur->right;
    }
}
```
- 先序/后序也有标准栈写法；**后序**最难（需记录右子是否已访问），或用"逆序输出 根右左 的翻转"技巧。
- Morris 遍历（$O(1)$ 额外空间、线索化思想）：见 [threaded-tree.md](threaded-tree.md)。

## 5. 遍历的现代/高阶用途

- **序列化与反序列化**：LeetCode 题；先序序列 + 空标记可还原。
- **欧拉序 / 时间戳（dfs 序）**：把子树变成连续区间，配合 [segment-tree.md](segment-tree.md)/[fenwick-tree.md](fenwick-tree.md) 做子树修改查询。
- **树的括号化**：后缀表达式求值 = 一棵表达式树的后序遍历。
- **树 DP 顺序**：子树聚合一律后序（先算儿子再算父亲）；依赖先父后子的用先序（如 B 树自顶向下搜索是"先序式路径遍历"）。

## 6. 复杂度与约定

- 每个节点访问常数次：$T(n)=\Theta(n)$；空间最坏 $O(n)$（链）/ 平均 $O(\log n)$。
- "先中后"之"序"指的是**根**的访问时机，用 DLR/LDR/LRD 记忆最稳。

## 相关概念

- 递归结构来源 [tree.md](tree.md)、[binary-tree.md](binary-tree.md)
- 遍历 ↔ 唯一确定树 → [expression-tree.md](expression-tree.md)
- 遍历与 BST 有序性 → [bst.md](bst.md)
- 线索化 = 遍历的指针优化 [threaded-tree.md](threaded-tree.md)
- 遍历序 → 区间化工具 [segment-tree.md](segment-tree.md)、[lca.md](lca.md)
