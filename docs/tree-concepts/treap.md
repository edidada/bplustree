# Treap（Tree + Heap，树堆）

## 1. 定义

**Treap** 是同时满足两种性质的二叉树：

1. **BST 性质（按键 key）**：中序为键的有序序列；
2. **堆序（按优先级 prio）**：每个节点的优先级 $\le$（最大堆）或 $\ge$（最小堆）其孩子——常用**最小堆**且 prio 越小越靠顶。

```
       key:  15, prio: 2
      /                \
  (10,5)              (25,4)
  /    \              /
(8,9) (12,7)       (20,11)
堆序: 2<5<9、2<4<11 等；键满足 BST；两者都成立 → treap
```

> 结构唯一性：**固定键集 + 固定优先级集** ⟹ 恰有一棵满足双性质的树（笛卡尔树是它的无键特例，见 [cartesian-tree.md](cartesian-tree.md)）。

## 2. 为什么随机优先级让它"几乎最优"

把优先级看成"插入时间戳的随机排列"，treap 正是**随机插入顺序 BST** 的等价形态：

- 随机 BST 的期望高度 $O(\log n)$（精确约 $1.39\log_2 n$，见 [bst.md](bst.md)）；
- treap 用随机堆序把"随机性从插入顺序中抽出来"，**与输入顺序解耦** → 恶意有序输入也无法构造退化（[degenerate-tree.md](degenerate-tree.md)）；
- 期望 $O(\log n)$ 操作；最坏仍是 $O(n)$（概率指数级小）。

## 3. 操作全部通过旋转

与 AVL 同用旋转，但"何时旋"由优先级驱动：

- **插入**：按 BST 找到位置挂新叶（prio 随机生成）→ 若其优先级高于父，**左/右旋上提**，直到堆序满足 → 至多沿路径旋到根 $O(\log n)$ 期望；
- **删除**：把目标节点**一直旋转到叶**（选优先级更小的孩子方向旋转）→ 删除叶；或标准 BST 删（用后继顶替）后处理堆序；
- **查找/前驱/后继**：纯 BST。

```c
/* 插入后修复：新节点太小就往上旋（最小堆版） */
node *fix(node *p, node *x) {          /* x 是 p 的子树中新点 */
    if (x->prio < p->prio) {           /* 需上提 */
        if (p->left == x) p = rotate_right(p);   /* 左孩子上提 */
        else              p = rotate_left(p);
    }
    return p;
}
```

## 4. 融合其他操作的扩展（硕士+）

- **区间/分裂合并 treap（FHQ treap / 无旋 treap）**：用 `split(root, key)`（按键切开成两棵）与 `merge(a,b)`（按 prio 合并）代替旋转；插入=split+merge、删除=split+split+merge。无旋版易于实现**区间翻转/序列维护**（文艺平衡树）与持久化（可持久化 treap）。
- **序统计**：加子树 size 字段 → 找第 k 小、rank。
- **可持久化**：treap 是少数能自然持久化的平衡 BST（每个版本共享子树、只复制路径 $O(\log n)$）。

## 5. 对比小结（何时用 treap）

| 需求 | 推荐 |
|---|---|
| 要可持久化 / 区间分裂合并 | treap（FHQ 无旋版） |
| 要最坏界、代码已验证 | AVL / 红黑（[avl-tree.md](avl-tree.md)） |
| 要摊还局部性、无需并发只读 | splay（[splay-tree.md](splay-tree.md)） |
| 随机性可接受、要实现最简 | treap（~60 行可写全） |

- 概率可靠性：期望分析不依赖真实随机数质量假设很松；可用 splitmix 等高质量 RNG。

## 6. 教材位置

- 本科/竞赛教材："随机化二叉搜索树"或 ADT 变体；
- Seidel & Aragon 1996 论文给出 treap 与随机 BST 的等价性与分析；
- 笛卡尔树（Vuillemin 1980）是其前身，[cartesian-tree.md](cartesian-tree.md)。

## 相关概念

- BST 基础与随机 BST [bst.md](bst.md)
- 旋转修复的同族：AVL/splay [avl-tree.md](avl-tree.md)、[splay-tree.md](splay-tree.md)
- 无键版=笛卡尔树 [cartesian-tree.md](cartesian-tree.md)
