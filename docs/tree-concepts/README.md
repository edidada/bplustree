# 树（Tree）概念知识库

> 数据结构/离散数学/算法中 **树** 相关概念的全集解析。
> 分层覆盖本科（国内主流教材：严蔚敏《数据结构(C语言版)》、邓俊辉《数据结构(清华 C++ 版)》）、硕士（CLRS《算法导论》、Knuth TAOCP、数据库教材）与博士/前沿专题（论文级结构：后缀树、R 树、斐波那契堆等）。
> 一个概念一个文件。位于本仓库，与仓库内磁盘 B+ 树实现（`lib/bplustree.c`）配套阅读。

---

## 0. 全库使用的统一约定

各教材对"根在第几层、空树多高"存在分歧，**这是初学者最容易混乱之处**。本知识库正文默认采用以下约定，并在相应文件内标注其他教材的差异：

| 概念 | 本库默认 | 严蔚敏(清华传统) | CLRS/Knuth(国际主流) | 邓俊辉(清华) |
|---|---|---|---|---|
| 层次 level(root) | 1 | 1（根为第 1 层） | 1 | 1 |
| 深度 depth(root) | **0**（从根往下数"边数"） | 1（与层次同义，数节点层号） | 0（边数） | 0（边的条数） |
| 高度 height(叶) | 0 | 1（数节点个数） | 0（边数） | 0 |
| 空树高度 | **-1** | 不定义/0 | -1 | -1 |
| 二叉"满"树 | 严格满 (strict/full) | 国内"满二叉树"= perfect | 见 `binary-tree-forms.md` | — |

> 根结点深度若从 1 起算，则几乎所有后续公式（如 AVL 平衡因子、红黑黑高）的"0/1 偏移"都要改写。CLRS 系约定在算法分析上最自洽，故作为默认；**阅读任何教材前先确认其 depth/height 起算**。

## 1. 目录索引（一个概念一个文件）

### A. 本科 · 树的基本概念与基础结构

| # | 文件 | 概念 | 一句话定位 |
|---|---|---|---|
| 1 | [tree.md](tree.md) | 树的定义 | 递归定义 vs 图论定义；树的等价性质 |
| 2 | [tree-terminology.md](tree-terminology.md) | 树术语总汇 | 根/叶/内点/父/子/兄弟/祖先/后代/子树 |
| 3 | [degree.md](degree.md) | 度 | 节点度、树的度；与 B 树"最小度数"的区别 |
| 4 | [level.md](level.md) | 层次/层数 | 按层编号；与深度/高度的起算对照 |
| 5 | [depth.md](depth.md) | 深度 | 从根往下数的距离 |
| 6 | [height.md](height.md) | 高度 | 从叶往上数的距离；空树 -1 |
| 7 | [ordered-tree.md](ordered-tree.md) | 有序/无序树 | 兄弟顺序是否参与结构 |
| 8 | [forest.md](forest.md) | 森林 | 森林、子树与树的递归结构 |
| 9 | [binary-tree.md](binary-tree.md) | 二叉树 | 二叉树及与树/森林的互转基础 |
| 10 | [binary-tree-forms.md](binary-tree-forms.md) | 二叉树特殊形态 | strict/complete/perfect/full 术语辨析 |
| 11 | [degenerate-tree.md](degenerate-tree.md) | 退化树/斜树 | 最坏形态，BST 退化的根源 |
| 12 | [tree-representation.md](tree-representation.md) | 树的存储表示 | 双亲/孩子/孩子兄弟(左孩子右兄弟) |
| 13 | [traversal.md](traversal.md) | 树的遍历 | 先序/中序/后序/层序；递归与栈模拟 |
| 14 | [expression-tree.md](expression-tree.md) | 表达式树 | 表达式 ↔ 树 ↔ 三种序列 |
| 15 | [bst.md](bst.md) | 二叉搜索树 | BST 性质、操作与平均/最坏代价 |
| 16 | [threaded-tree.md](threaded-tree.md) | 线索二叉树 | 用空指针加速遍历 |
| 17 | [huffman-tree.md](huffman-tree.md) | 哈夫曼树 | 最优二叉树、WPL、前缀编码 |
| 18 | [binary-heap.md](binary-heap.md) | 二叉堆 | 完全二叉树的顺序存储与堆序 |
| 19 | [tournament-tree.md](tournament-tree.md) | 胜者树/败者树 | 锦标赛树，外部排序归并 |
| 20 | [union-find.md](union-find.md) | 并查集 | 用树/森林表示的集合合并 |

### B. 硕士 · 平衡搜索树与高级堆

| # | 文件 | 概念 | 一句话定位 |
|---|---|---|---|
| 21 | [avl-tree.md](avl-tree.md) | AVL 树 | 高度平衡树，平衡因子 -1/0/+1 |
| 22 | [red-black-tree.md](red-black-tree.md) | 红黑树 | 平衡 BST，与 2-3-4 树的对应 |
| 23 | [23-tree.md](23-tree.md) | 2-3 树/2-3-4 树 | 多路平衡树，B 树的特例 |
| 24 | [b-tree.md](b-tree.md) | B 树 | 磁盘多路平衡搜索树 |
| 25 | [bplus-tree.md](bplus-tree.md) | B+ 树 | B 树变体，本仓库实现对象 |
| 26 | [splay-tree.md](splay-tree.md) | 伸展树 | 摊还平衡的自我调整树 |
| 27 | [treap.md](treap.md) | Treap | 二叉搜索树 + 随机堆序 |
| 28 | [scapegoat-tree.md](scapegoat-tree.md) | 替罪羊树 | 局部重建式平衡（重量平衡思想） |
| 29 | [leftist-heap.md](leftist-heap.md) | 左偏堆 | 可并堆家族 |
| 30 | [binomial-heap.md](binomial-heap.md) | 二项树/二项堆 | 二项树的合并堆 |
| 31 | [fibonacci-heap.md](fibonacci-heap.md) | 斐波那契堆 | 摊还最优的可并堆 |

### C. 硕博 · 字典树 / 索引 / 区间结构

| # | 文件 | 概念 | 一句话定位 |
|---|---|---|---|
| 32 | [trie.md](trie.md) | 字典树 | Trie/前缀树/Patricia/基数树 |
| 33 | [suffix-tree.md](suffix-tree.md) | 后缀树 | 字符串全后缀索引 |
| 34 | [segment-tree.md](segment-tree.md) | 线段树 | 区间查询/更新的树形结构 |
| 35 | [fenwick-tree.md](fenwick-tree.md) | 树状数组 | 前缀和的 BIT 结构 |
| 36 | [cartesian-tree.md](cartesian-tree.md) | 笛卡尔树 | 堆序 + 中序的数组树 |
| 37 | [kd-tree.md](kd-tree.md) | K-D 树 | 空间划分树（KD/四叉/八叉） |
| 38 | [r-tree.md](r-tree.md) | R 树 | 空间对象索引树 |

### D. 博/专题 · 图论中的树与树算法

| # | 文件 | 概念 | 一句话定位 |
|---|---|---|---|
| 39 | [graph-tree.md](graph-tree.md) | 自由树/图论树 | 连通无环图，树的充要条件 |
| 40 | [spanning-tree.md](spanning-tree.md) | 生成树/MST | 生成树、生成森林、最小生成树 |
| 41 | [prufer-sequence.md](prufer-sequence.md) | Prufer 序列 | 树 ↔ 序列双射，Cayley 定理 |
| 42 | [tree-diameter.md](tree-diameter.md) | 树的直径 | 最长路径与两遍 DFS |
| 43 | [tree-centroid.md](tree-centroid.md) | 树的重心 | 删点后子树最小的点 |
| 44 | [tree-isomorphism.md](tree-isomorphism.md) | 树的同构 | 根树/自由树同构判定 (AHU) |
| 45 | [lca.md](lca.md) | 最近公共祖先 | LCA 及倍增/欧拉/重链方法 |

---

## 2. 阅读路线建议

- **入门（对应本科一学期）**：1–20 按序号读。先吃透 `tree-terminology`、`degree`、`level/depth/height` 四个基础词条（很多公式混乱来自起算约定），再学二叉树与遍历，然后 BST→二叉堆→哈夫曼→并查集。
- **进阶（数据结构/算法研究生课、面试）**：21–31。重点：AVL 与红黑树（平衡机制对比）、B/B+ 树（磁盘视角，衔接本仓库 `lib/bplustree.c`）。
- **专题/研究（读论文前扫盲）**：32–38（索引与字符串结构）、39–45（图论与树上算法）。
- 每篇末尾的「相关概念」提供跳转链接。

## 3. 与仓库实现的衔接

本仓库磁盘 B+ 树（`lib/bplustree.c`）是概念篇 24/25 的活例：

- 节点 = 一个磁盘 block，页大小（如 4096 B）对应 B 树的 **order/块因子**；
- 叶节点间 prev/next 兄弟链 → B+ 树的范围遍历设计；
- `.boot` 元数据（root 偏移、block 大小、文件大小、free_blocks）→ B 树在磁盘上的"元数据/空闲块管理"；
- 缓存仅 5 个 block → 对应概念篇中"缓存管理"（对比工业级页缓存）。
