# 度（Degree）

## 1. 定义

在有根树中：

- **节点的度（degree）**：该节点拥有的 **子节点的个数**，记 $\deg(v)=|\mathrm{ch}(v)|$。
- **树的度**：树中所有节点度的 **最大值**，记 $\Delta(T)=\max_{v\in T}\deg(v)$。

```
      (a)
     / | \          deg(a)=3
   (b)(c)(d)        deg(b)=1（只有 e）
     |              deg(e)=0 → e 是叶
    (e)
  树的度 = max{3,1,0} = 3
```

## 2. 由度导出的角色

- $\deg(v)=0$：**叶（leaf）**；
- $\deg(v)>0$：**内部/分支节点（internal / branch node）**；
- 二叉树要求 $\Delta(T)\le 2$，且孩子区分为左/右孩子（有序，见 [binary-tree.md](binary-tree.md)）。

## 3. 关键性质与公式

1. **握手引理的有根树版**：对 $n\ge 1$ 个节点，
   $$\sum_{v\in T}\deg(v) = n - 1$$
   因为除根外每个节点恰被一条"父→子"边指向。这是"树有 $n-1$ 条边"的节点度表述。

2. **二叉树叶数公式**：设二叉树中度为 0/1/2 的节点数为 $n_0,n_1,n_2$，则
   $$n_0 = n_2 + 1$$
   证明：由 (1) 得 $2n_2+n_1 = n-1 = (n_0+n_1+n_2)-1 \Rightarrow n_0=n_2+1$。这是最常考的树公式之一（放 [binary-tree.md](binary-tree.md) 讲其含义）。

3. **哈夫曼树**中只出现度为 0 或 2 的节点（无度 1 节点）：$n_1=0$，故总节点数 $= 2n_0-1$（见 [huffman-tree.md](huffman-tree.md)）。

## 4. 度的两个常见歧义（务必区分）

1. **度 = 子节点数 vs 度 = 关联边数**：在有根树语境默认前者；在无向图（[graph-tree.md](graph-tree.md)）中 $\deg(v)$ 是"关联的边数"。根树中两者只差 $\pm1$（根：子树度 +0/边度一致；叶在图论意义下度是 1 而不是 0）——**讨论"叶的度"前先确认语境**。
2. **树的度 $\Delta$ vs B 树的"最小度数 $t$"**：B 树教材（CLRS）里的"最小度数"$t$ 不是上面 $\deg(v)$ 的语义，而是"每个内部节点至少含 $t-1$ 个键、最多 $2t-1$ 个键"的规模参数；国内教材常用"阶 $m$"而不用 $t$。详见 [b-tree.md](b-tree.md) 的记法对照表。

## 5. 各层级视角

- **本科**：度是判定"叶/内点"的唯一依据，也是分析二叉树形态（$n_0=n_2+1$）与链式存储空指针数的工具（$n+1$ 个空链，见 [threaded-tree.md](threaded-tree.md)）。
- **研究生**：度影响树的"分支因子"，多路树（B 树）通过提高 $\Delta$ 降低高度与磁盘 I/O；平衡二叉树的旋转本质是维持 $\Delta$ 相同下重排层高。
- **更高视角**：在组合枚举中，树的度序列（degree sequence）有刻画定理（图论：Prüfer / 度序列条件），见 [graph-tree.md](graph-tree.md)、[prufer-sequence.md](prufer-sequence.md)。

## 相关概念

- 叶/内部节点 [tree-terminology.md](tree-terminology.md)
- 二叉树与 $n_0=n_2+1$ [binary-tree.md](binary-tree.md)
- 树的度决定高度上界（对数底）→ [b-tree.md](b-tree.md)
- 图的度 [graph-tree.md](graph-tree.md)
