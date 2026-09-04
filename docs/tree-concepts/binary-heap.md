# 二叉堆（Binary Heap / 优先队列）

## 1. 定义

**二叉堆**是一棵**完全二叉树**（[complete](binary-tree-forms.md)）+ **堆序**约束，通常用数组顺序存储：

- **最大堆**：每个节点 $\ge$ 其孩子（堆顶最大）；
- **最小堆**：每个节点 $\le$ 其孩子（堆顶最小）。

```
最小堆（数组 [1,3,2,6,4,5] 按层序存放）
        1            下标:  父 floor(i/2)
       / \                 左子 2i
      3   2                右子 2i+1
     / \ / \
    6  4 5
```
- 完全性保证 **高度 $\lfloor\log_2 n\rfloor$** 且**无数组空洞** → 空间 $O(1)$ 额外。
- 堆**不是搜索结构**：只保证"顶"极值，不能 $O(\log n)$ 搜任意键（除非另加索引）。

## 2. 核心操作与上/下滤

| 操作 | 实现 | 复杂度 |
|---|---|---|
| 取最值 peek | 数组首 | $O(1)$ |
| 插入 push | 尾部放入 + **上滤（sift-up）** | $O(\log n)$ |
| 弹顶 pop | 顶换尾、删尾 + **下滤（sift-down）** | $O(\log n)$ |
| 建堆 heapify | 从 $\lfloor n/2\rfloor$ 起逐个下滤 | $O(n)$（不是 $O(n\log n)$，求和收敛） |
| 改键 decrease/increase-key | 上滤/下滤 | $O(\log n)$（Dijkstra/Prim 用） |

```c
/* 上滤：新节点太"小"（最小堆）就与父交换往上爬 */
void sift_up(int h[], int i) {
    while (i > 1 && h[i] < h[i / 2]) {
        swap(&h[i], &h[i / 2]);
        i /= 2;
    }
}
```

## 3. 关键性质/证明

1. **建堆 $O(n)$**：$\sum_{h} (\text{该层节点数})\times O(h)$ 收敛为 $O(n)$。
2. 高度 $\lfloor\log_2 n\rfloor$，所有操作以高度为界。
3. 数组中节点 $i$ 的孩子 `2i, 2i+1` 仅当不超过 $n$。
4. 叶子下标范围：$i>\lfloor n/2\rfloor$ 是叶。

## 4. 教材/应用位置

- **本科**：优先队列、堆排序（建堆 $O(n)$ + 反复 pop 得有序 = $O(n\log n)$ 就地排序）、Huffman 与 Dijkstra/Prim 的取最小。
- **硕士**：堆是"抽象优先队列 ADT"的一种实现；D 叉堆、配对堆用于图算法；"堆与平衡 BST 都能当优先队列，BST 还支持删除任意元素"。
- **博士/算法理论**：堆与排序下界、二进制/二项/斐波那契/配对堆的摊还对比见 [binomial-heap.md](binomial-heap.md)、[fibonacci-heap.md](fibonacci-heap.md)。

## 5. 堆 vs 相关结构（快速区分）

| 结构 | 允许"找任意键" | 合并 | 备注 |
|---|---|---|---|
| 二叉堆 | 否（数组索引 ≠ 键） | 需合并两堆时重建 | 教学与工程基础 |
| 左偏堆 | 否 | $O(\log n)$ | [leftist-heap.md](leftist-heap.md) |
| 二项堆 | 否 | $O(\log n)$ | [binomial-heap.md](binomial-heap.md) |
| 斐波那契堆 | 否 | $O(1)$ 摊还 | [fibonacci-heap.md](fibonacci-heap.md) |
| 平衡 BST | **是**（中序有序） | 建树 $O(n)$ | 优先队列超集 |

> B+ 树里也用到"每块尽量保持半满以上"——堆/多叉树对"下滤合并"的边界处理思想（上滤到根、兄弟不足合并）与 B 树的删除合并一脉相承，可对比 [b-tree.md](b-tree.md)。

## 相关概念

- 完全二叉树与数组存树 [binary-tree-forms.md](binary-tree-forms.md)
- 贪心应用：Huffman [huffman-tree.md](huffman-tree.md)
- 可并堆家族 [leftist-heap.md](leftist-heap.md)、[binomial-heap.md](binomial-heap.md)、[fibonacci-heap.md](fibonacci-heap.md)
- 多叉平衡删除合并类比 [b-tree.md](b-tree.md)
