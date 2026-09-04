# 二叉树（Binary Tree）

## 1. 定义

**二叉树**是每个节点**至多有两个孩子**、且孩子**区分左右**的有序树。用递归定义：

> 二叉树是节点集 $T$：要么为空，要么由**根**及**左子树**、**右子树**两棵（可为空）二叉树组成。

与"度为 2 的树"本质区别：二叉树的孩子**有左右之分**，即使只有一棵子树也须指明是左还是右（见 [ordered-tree.md](ordered-tree.md)）。

```
       (5)
      /   \
   (3)     (7)        左子树、右子树均可能为空
  /  \       \
(1)  (4)    (9)
```

## 2. 二叉树的基本性质（全部本科必考）

1. 第 $i$ 层至多 $2^{i-1}$ 个节点（[level.md](level.md)）。
2. 深度为 $k$（根 0 起）的二叉树至多 $2^{k+1}-1$ 个节点。
3. 叶/双分支计数关系：$n_0 = n_2 + 1$（度为 0 与度为 2 节点数关系，见 [degree.md](degree.md)）。
4. $n$ 个节点的**不同形态**二叉树共有 Catalan 数 $C_n=\dfrac{1}{n+1}\dbinom{2n}{n}$ 棵。
5. 完全二叉树顺序编号（父 $\lfloor i/2\rfloor$、左子 $2i$、右子 $2i+1$）见 [binary-tree-forms.md](binary-tree-forms.md) 与 [binary-heap.md](binary-heap.md)。

## 3. 存储

1. **顺序存储（数组）**：仅适合完全二叉树（按层序编号填入数组），否则大量空洞。父/子用下标公式。B+ 树的页内线性区与此思想类似。
2. **链式存储**：三域（data、left、right），$n$ 个节点用 $n$ 个结点、**$2n$ 个链域中 $n+1$ 个为空**（非空边恰 $n-1$ 条）——空链域的再利用就是线索树（[threaded-tree.md](threaded-tree.md)）。
   ```c
   struct bnode { key_t key; struct bnode *left, *right; };
   ```
3. 三叉链（加 parent 指针）便于回溯/旋转/删父场景（AVL、splay 的某些实现）。

## 4. 遍历与性质判定

- 先序/中序/后序/层序：见 [traversal.md](traversal.md)。
- **由先序+中序（或后序+中序）可唯一确定二叉树**；仅先序+后序一般不能唯一确定。
- 判定工具：先序/中序把树序列化后，二叉搜索树（BST）的**中序必有序**，这是 [bst.md](bst.md) 的一切基础。

## 5. 二叉树上的递归范式

三个经典递推（子问题 = 左/右子树）：

- 高度：$h(v)=1+\max(h_L,h_R)$
- 规模：$s(v)=1+s_L+s_R$
- 直径/最长路径：跨过 $v$ 的候选 = $h_L+h_R+2$（[tree-diameter.md](tree-diameter.md)）

## 6. 教材视角

- **本科**：二叉树是整个树章节的中枢——遍历、存储、线索化、二叉搜索树、哈夫曼全在其上展开。
- **硕士**：二叉树是"可分割二分"递归结构的最优载体，BST/AVL/红黑/线段树都是"在二叉树上加约束与信息"。
- **博士/组合**：二叉树计数与 Catalan、随机二叉树高度渐近 $2\sqrt{\pi n}$、生成树对偶等。

## 相关概念

- 二叉树形态术语辨析（strict/complete/perfect/full）[binary-tree-forms.md](binary-tree-forms.md)
- 有序树与二叉树双射 [ordered-tree.md](ordered-tree.md)
- 存储表示 [tree-representation.md](tree-representation.md)、[binary-heap.md](binary-heap.md)
- 遍历 [traversal.md](traversal.md)、BST [bst.md](bst.md)
