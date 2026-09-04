/*
 * Same-machine micro benchmark for this repo's disk-backed B+ tree.
 * Paired with sqlite_bench.c: both run the same load so the numbers are
 * directly comparable. Usage (from the repo root):
 *   gcc -O2 -Wall bench/bplus_bench.c lib/bplustree.c -Ilib -o bplus_bench
 *   ./bplus_bench
 *
 * Results used by: bplus_industrial_comparison.md (repo root)
 */
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>

#include "bplustree.h"

#define N 1000000

static unsigned long rng_state = 88172645463325252UL;

static unsigned long rng(void)
{
        rng_state = rng_state * 6364136223846793005UL + 1442695040888963407UL;
        return rng_state >> 33;
}

static double now(void)
{
        struct timeval tv;
        gettimeofday(&tv, NULL);
        return tv.tv_sec + tv.tv_usec / 1e6;
}

static void rm_index(void)
{
        remove("/tmp/bplus_bench.index");
        remove("/tmp/bplus_bench.index.boot");
}

int main(void)
{
        struct bplus_tree *tree;
        long i;
        double t0, t1, t2, t3;
        unsigned long hits = 0, span_rows = 0;

        rm_index();
        tree = bplus_tree_init("/tmp/bplus_bench.index", 4096);
        if (tree == NULL) {
                return 1;
        }

        /* 1) sequential insert N keys with data==key */
        t0 = now();
        for (i = 1; i <= N; i++) {
                bplus_tree_put(tree, (key_t)i, i);
        }
        t1 = now();

        /* 2) random point query over the same key space */
        for (i = 0; i < N; i++) {
                if (bplus_tree_get(tree, (key_t)(rng() % N + 1)) != -1) {
                        hits++;
                }
        }
        t2 = now();

        /* 3) range queries, window up to 512 wide */
        for (i = 0; i < N / 4; i++) {
                key_t k1 = (key_t)(rng() % N + 1);
                key_t k2 = (key_t)(k1 + (rng() % 512) + 1);
                span_rows += (unsigned long)bplus_tree_get_range(tree, k1, k2);
        }
        t3 = now();

        bplus_tree_deinit(tree);
        rm_index();

        printf("sequential insert      : %ld keys in %.3fs -> %.0f keys/s\n",
               (long)N, t1 - t0, N / (t1 - t0));
        printf("random point query     : %ld keys in %.3fs -> %.0f keys/s (hits %lu)\n",
               (long)N, t2 - t1, N / (t2 - t1), hits);
        printf("range query (<=512)    : %ld queries in %.3fs -> %.0f q/s (last=%lu)\n",
               (long)N / 4, t3 - t2, (N / 4) / (t3 - t2), span_rows);
        return 0;
}
