/*
 * File-driven query test for the disk-backed B+ tree.
 *
 * Unlike bplustree_demo.c (interactive stdin), every input comes from two
 * plain-text config files given on the command line:
 *
 *   ./bplustree_query_test <build.conf> <search.conf>
 *
 * build.conf  (how the tree is constructed):
 *   line 1            : data index file path for bplus_tree_init()
 *   line 2            : block size in bytes (positive power of two)
 *   following lines   : keys to insert (whitespace separated, >= 1)
 *
 * search.conf (what is queried afterwards):
 *   range <k1> <k2>   : run bplus_tree_get_range() on [min(k1,k2),max(k1,k2)]
 *   in <k1> [<k2>..]  : membership check of each key via bplus_tree_get()
 *
 * The driver mirrors the inserted keys in memory, checks every query result
 * against the mirror and exits non-zero as soon as a check fails, so it can
 * be wired into CTest. The tree is built, closed, re-opened from the index
 * file (exercising the persisted on-disk state) and only then queried.
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "bplustree.h"

/* ------------------ minimal int hash set (key mirror) ------------------ */

struct int_set {
        int *table;     /* open addressing, 0 marks an empty slot */
        int cap;        /* power of two */
        int count;
};

static unsigned int iset_hash(int key)
{
        return (unsigned int)key * 2654435761u;
}

static int iset_contains(const struct int_set *set, int key)
{
        int idx = (int)(iset_hash(key) & (unsigned int)(set->cap - 1));

        while (set->table[idx] != 0) {
                if (set->table[idx] == key) {
                        return 1;
                }
                idx = (idx + 1) & (set->cap - 1);
        }
        return 0;
}

static int iset_add(struct int_set *set, int key);

static int iset_grow(struct int_set *set, int new_cap)
{
        struct int_set bigger;
        int i;

        bigger.cap = new_cap;
        bigger.count = 0;
        bigger.table = calloc((size_t)new_cap, sizeof(*bigger.table));
        if (bigger.table == NULL) {
                return -1;
        }
        for (i = 0; i < set->cap; i++) {
                if (set->table[i] != 0 && iset_add(&bigger, set->table[i]) < 0) {
                        free(bigger.table);
                        return -1;
                }
        }
        free(set->table);
        *set = bigger;
        return 0;
}

static int iset_add(struct int_set *set, int key)
{
        int idx;

        if (iset_contains(set, key)) {
                return 0;
        }
        if ((set->count + 1) * 10 >= set->cap * 7) {
                if (iset_grow(set, set->cap << 1) < 0) {
                        return -1;
                }
        }
        idx = (int)(iset_hash(key) & (unsigned int)(set->cap - 1));
        while (set->table[idx] != 0) {
                idx = (idx + 1) & (set->cap - 1);
        }
        set->table[idx] = key;
        set->count++;
        return 0;
}

static int iset_init(struct int_set *set)
{
        set->cap = 64;
        set->count = 0;
        set->table = calloc((size_t)set->cap, sizeof(*set->table));
        return set->table != NULL ? 0 : -1;
}

static void iset_free(struct int_set *set)
{
        free(set->table);
        set->table = NULL;
}

static int iset_largest_in_range(const struct int_set *set, int min, int max)
{
        int found = 0;
        int best = 0;
        int i;

        for (i = 0; i < set->cap; i++) {
                int key = set->table[i];

                if (key >= min && key <= max && (!found || key > best)) {
                        found = 1;
                        best = key;
                }
        }
        return found ? best : -1;
}

/* ------------------------------- helpers ------------------------------- */

static int parse_positive_int(const char *tok, int *out)
{
        char *end = NULL;
        long val;

        if (tok == NULL || *tok == '\0') {
                return -1;
        }
        val = strtol(tok, &end, 10);
        if (*end != '\0' || val <= 0 || val > 0x7fffffffL) {
                return -1;
        }
        *out = (int)val;
        return 0;
}

static void rm_index_files(const char *index_file)
{
        char boot_file[1024 + 8];

        remove(index_file);
        snprintf(boot_file, sizeof(boot_file), "%s.boot", index_file);
        remove(boot_file);
}

/* build.conf: line 1 index path, line 2 block size, rest keys to insert */
struct build_cfg {
        char index_file[1024];
        int block_size;
        struct int_set keys;
};

static int load_build_cfg(const char *path, struct build_cfg *cfg)
{
        FILE *fp;
        char line[4096];
        int stage = 0;
        int lineno = 0;

        memset(cfg, 0, sizeof(*cfg));
        if (iset_init(&cfg->keys) < 0) {
                fprintf(stderr, "out of memory\n");
                return -1;
        }
        fp = fopen(path, "r");
        if (fp == NULL) {
                fprintf(stderr, "%s: %s\n", path, strerror(errno));
                iset_free(&cfg->keys);
                return -1;
        }
        while (fgets(line, sizeof(line), fp) != NULL) {
                char *tok;

                lineno++;
                tok = strtok(line, " \t\r\n");
                if (tok == NULL || tok[0] == '#') {
                        continue;
                }
                if (stage == 0) {
                        if (strlen(tok) >= sizeof(cfg->index_file)) {
                                fprintf(stderr, "%s:%d: index file path too long\n",
                                        path, lineno);
                                goto fail;
                        }
                        strcpy(cfg->index_file, tok);
                        stage = 1;
                } else if (stage == 1) {
                        if (parse_positive_int(tok, &cfg->block_size) < 0 ||
                            (cfg->block_size & (cfg->block_size - 1)) != 0) {
                                fprintf(stderr,
                                        "%s:%d: block size must be a power of two\n",
                                        path, lineno);
                                goto fail;
                        }
                        stage = 2;
                } else {
                        do {
                                int key;

                                if (tok[0] == '#') {
                                        break;
                                }
                                if (parse_positive_int(tok, &key) < 0 ||
                                    iset_add(&cfg->keys, key) < 0) {
                                        fprintf(stderr, "%s:%d: bad insert key \"%s\"\n",
                                                path, lineno, tok);
                                        goto fail;
                                }
                        } while ((tok = strtok(NULL, " \t\r\n")) != NULL);
                }
        }
        fclose(fp);
        if (stage < 2) {
                fprintf(stderr, "%s: expected index path and block size first\n", path);
                iset_free(&cfg->keys);
                return -1;
        }
        return 0;

fail:
        fclose(fp);
        iset_free(&cfg->keys);
        return -1;
}

/* --------------------- search.conf execution loop ---------------------- */

static void report(int pass, const char *fmt, long v1, long v2)
{
        printf("%s %s [tree=%ld expected=%ld]\n", pass ? "PASS" : "FAIL",
               fmt, v1, v2);
}

static int run_search_conf(const char *path, struct bplus_tree *tree,
                           const struct int_set *keys)
{
        FILE *fp;
        char line[4096];
        int lineno = 0;
        int checks = 0;
        int failed = 0;

        fp = fopen(path, "r");
        if (fp == NULL) {
                fprintf(stderr, "%s: %s\n", path, strerror(errno));
                return -1;
        }
        while (fgets(line, sizeof(line), fp) != NULL) {
                char *op;
                int pass;

                lineno++;
                op = strtok(line, " \t\r\n");
                if (op == NULL || op[0] == '#') {
                        continue;
                }
                if (strcmp(op, "range") == 0) {
                        char *tok1 = strtok(NULL, " \t\r\n");
                        char *tok2 = strtok(NULL, " \t\r\n");
                        int k1, k2, min, max, expected;
                        long actual;

                        if (parse_positive_int(tok1, &k1) < 0 ||
                            parse_positive_int(tok2, &k2) < 0) {
                                fprintf(stderr, "%s:%d: range needs two positive keys\n",
                                        path, lineno);
                                failed++;
                                continue;
                        }
                        min = k1 <= k2 ? k1 : k2;
                        max = min == k1 ? k2 : k1;
                        expected = iset_largest_in_range(keys, min, max);
                        actual = bplus_tree_get_range(tree, k1, k2);
                        pass = actual == (long)expected;
                        report(pass, "range", actual, expected);
                        checks++;
                        failed += !pass;
                } else if (strcmp(op, "in") == 0) {
                        char *tok = strtok(NULL, " \t\r\n");

                        while (tok != NULL) {
                                int key;
                                long data;
                                int found, expect;

                                if (parse_positive_int(tok, &key) < 0) {
                                        fprintf(stderr, "%s:%d: bad in key \"%s\"\n",
                                                path, lineno, tok);
                                        failed++;
                                        tok = strtok(NULL, " \t\r\n");
                                        continue;
                                }
                                data = bplus_tree_get(tree, key);
                                found = data != -1;
                                expect = iset_contains(keys, key);
                                pass = found == expect;
                                report(pass, "in", data, (long)(expect ? 1 : 0));
                                checks++;
                                failed += !pass;
                                tok = strtok(NULL, " \t\r\n");
                        }
                } else {
                        fprintf(stderr, "%s:%d: unknown op \"%s\" (use range|in)\n",
                                path, lineno, op);
                        failed++;
                }
        }
        fclose(fp);
        printf("### query test: %d/%d checks passed\n", checks - failed, checks);
        return failed ? 1 : 0;
}

int main(int argc, char *argv[])
{
        struct build_cfg cfg;
        struct bplus_tree *tree;
        int rc = 0;
        int i;

        if (argc != 3) {
                fprintf(stderr,
                        "usage: %s <build.conf> <search.conf>\n"
                        "  build.conf : index file path, block size, keys to insert\n"
                        "  search.conf: 'range <k1> <k2>' and 'in <k1> [<k2> ...]' lines\n",
                        argv[0]);
                return 2;
        }
        if (load_build_cfg(argv[1], &cfg) < 0) {
                return 2;
        }

        /* Start from a clean index file: its block config is immutable. */
        rm_index_files(cfg.index_file);

        /* Phase 1: construct the tree from build.conf and persist it. */
        tree = bplus_tree_init(cfg.index_file, cfg.block_size);
        if (tree == NULL) {
                fprintf(stderr, "failed to init tree on %s\n", cfg.index_file);
                iset_free(&cfg.keys);
                return 1;
        }
        for (i = 0; i < cfg.keys.cap; i++) {
                if (cfg.keys.table[i] != 0) {
                        bplus_tree_put(tree, cfg.keys.table[i], cfg.keys.table[i]);
                }
        }
        bplus_tree_deinit(tree);

        /* Phase 2: re-open the on-disk tree, then answer search.conf. */
        tree = bplus_tree_init(cfg.index_file, cfg.block_size);
        if (tree == NULL) {
                fprintf(stderr, "failed to re-open tree on %s\n", cfg.index_file);
                iset_free(&cfg.keys);
                return 1;
        }
        rc = run_search_conf(argv[2], tree, &cfg.keys);
        bplus_tree_deinit(tree);

        rm_index_files(cfg.index_file);
        iset_free(&cfg.keys);
        return rc;
}
