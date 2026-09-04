# 二叉树特殊形态（Strict / Complete / Perfect / Full —— 术语辨析）

> "满二叉树""完全二叉树""满树"是**翻译灾难**最大的术语组。本文用英文原名 + 中文习惯标注，务必对照英文记忆。

## 1. 四种形态

| 中文常用名 | 英文 | 定义 | 等价条件 |
|---|---|---|---|
| **严格二叉树**（又称满/真/正则，F 树） | **strict / proper / full** | 每个节点度为 0 或 2（无度为 1 的节点） | 有 $n_0$ 个叶时总节点 $=2n_0-1$ |
| **完全二叉树** | **complete** | 除最后一层外各层都满，最后一层节点**从左到右连续**排布 | 层序编号 1..n 与同高满树前缀一致 |
| **完美二叉树**（国内传统"满二叉树"） | **perfect** | 所有层全满；即 complete 且最后一层也满 | 高 $k$ 时节点 $=2^{k+1}-1$，叶全在同一层 |
| **近似完全/次完全**（罕见，勿考） | **almost complete** | 旧讲义对 complete 的旧称 | — |

```
strict（每个内部节点恰两子）:
         (a)
        /   \
      (b)   (c)
     /  \
  (d)  (e)          ← 不要求满层、不要求叶同深

complete（逐层排满、最后一层从左到右连续）:
         (a)
        /   \
      (b)   (c)
     /  \   /
  (d) (e)(f)        ← 只缺最右下角的位

perfect（perfect = complete 且最后一层也满）:
         (a)
        /   \
      (b)   (c)
     /  \   /  \
  (d)(e) (f)(g)     ← 高 2 的 perfect：2^3−1 = 7 个节点
```

### 容易混淆的关键区分

- **full**（英式 strict/proper）只关心"每个内部节点 0/2 子"，不要求满层；
- **perfect** 要求全部层满（教科书常叫"满二叉树"）；
- **complete** 是"perfect 从最后一层右边砍掉若干叶子"（但必须连续靠左）。
- 因此 `complete ⊇ perfect`：perfect 是 complete 的特例；strict 与 complete 无包含关系。

> 国内（严蔚敏等）所谓"满二叉树" = perfect；所谓"完全二叉树" = complete。翻译时看到英文文档的 "full binary tree" 千万核对：TAOCP 里 full = 所有节点 0 或 2 子；有些老资料里 full = perfect。**以定义优先于名称**。

## 2. 完全二叉树：最重要的两个性质

1. **层序编号 ↔ 数组下标**：把 complete 的 $n$ 个节点从上到下、从左到右编号 1..n，则
   - 父：$\lfloor i/2\rfloor$
   - 左子：$2i$（$\le n$ 才存在）
   - 右子：$2i+1$（$\le n$ 才存在）
   - 叶：编号 $>\lfloor n/2\rfloor$
2. **顺序存储无空洞** → 二叉堆（[binary-heap.md](binary-heap.md)）直接用数组。

## 3. 各结构的"满/完全"形容词别混

| 结构 | 常用形容词 | 含义 |
|---|---|---|
| 二叉堆 | "完全"（complete） | 一定是 complete 树 |
| 严格二叉树 | "满" | 只指 0/2 子 |
| 满堆 | — | 不常用 |
| B 树节点 | "满"（full） | 键数达上限将分裂（[b-tree.md](b-tree.md)） |
| AVL | — | 不要求 complete |

## 4. 高度与节点数不等式（公式速查，n = 节点数）

- complete / perfect 的高（边数计，perfect 高 $k$）节点数：$2^{k+1}-1$（perfect）、$[2^{k},\ 2^{k+1}-1]$（complete 高 $k$）。
- 任意二叉树：$n \le 2^{k+1}-1 \Rightarrow k \ge \lceil \log_2(n+1)\rceil -1$。
- **推论（教材最爱考）**：高 $h$ 的二叉树至少 $h+1$ 个节点（链）；perfect 达到最大 $2^{h+1}-1$。

## 5. 扩充二叉树 / 外部节点（研究生术语）

把二叉树每个空孩子替换为**外部节点（external / 方形）**，原节点称内部节点——得到 **扩充二叉树（extended binary tree）**。由此：
- external 节点 = "NIL 哨兵" 的显式化，使所有原内点度为 2，叶公式化为 $n_0 = n_2 +1$ 的**内部/外部**版本：外部节点数 $=$ 内部节点数 $+1$；
- 它是 AVL 判定、红黑树 NIL、以及哈夫曼"扩充二叉树 / 外路径长"（[huffman-tree.md](huffman-tree.md)）的标准框架。

## 相关概念

- 二叉树 [binary-tree.md](binary-tree.md)
- 完全二叉树 → 顺序存储二叉堆 [binary-heap.md](binary-heap.md)
- 叶公式 $n_0=n_2+1$ [degree.md](degree.md)
- 严格二叉与哈夫曼 [huffman-tree.md](huffman-tree.md)
