/*
 * Same-machine, same-load C benchmark for SQLite 3.45.1 B-tree storage
 * (PK INTEGER table; data column mirrors key). Paired with bplus_bench.c
 * so the numbers are directly comparable. Requires sqlite3 C API
 * (libsqlite3-dev on Debian/Ubuntu). Usage (from the repo root):
 *   gcc -O2 -Wall bench/sqlite_bench.c -lsqlite3 -o sqlite_bench
 *   ./sqlite_bench
 *
 * Results used by: docs/bplus_industrial_comparison.md
 */
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <sys/time.h>

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

static void rm_db(void)
{
        remove("/tmp/sqlite_bench.db");
        remove("/tmp/sqlite_bench.db-wal");
        remove("/tmp/sqlite_bench.db-shm");
}

int main(void)
{
        sqlite3 *db;
        sqlite3_stmt *stmt;
        int rc;
        long i;
        double t0, t1, t2, t3, t4;
        unsigned long hits = 0, rows = 0;
        char *err = NULL;

        rm_db();
        if (sqlite3_open("/tmp/sqlite_bench.db", &db) != SQLITE_OK) {
                fprintf(stderr, "open: %s\n", sqlite3_errmsg(db));
                return 1;
        }
        sqlite3_exec(db, "PRAGMA journal_mode=WAL", 0, 0, &err);
        sqlite3_exec(db, "PRAGMA synchronous=NORMAL", 0, 0, &err);
        sqlite3_exec(db, "CREATE TABLE t(k INTEGER PRIMARY KEY, v INTEGER)", 0, 0, &err);
        sqlite3_exec(db, "BEGIN", 0, 0, &err);

        /* 1) sequential insert N keys (single transaction) */
        sqlite3_prepare_v2(db, "INSERT INTO t(k,v) VALUES(?,?)", -1, &stmt, NULL);
        t0 = now();
        for (i = 1; i <= N; i++) {
                sqlite3_bind_int64(stmt, 1, i);
                sqlite3_bind_int64(stmt, 2, i);
                sqlite3_step(stmt);
                sqlite3_reset(stmt);
        }
        sqlite3_finalize(stmt);
        sqlite3_exec(db, "COMMIT", 0, 0, &err);
        t1 = now();

        /* 2) random point query through the PK index */
        sqlite3_prepare_v2(db, "SELECT v FROM t WHERE k=?", -1, &stmt, NULL);
        t2 = now();
        for (i = 0; i < N; i++) {
                sqlite3_bind_int64(stmt, 1, (sqlite3_int64)(rng() % N + 1));
                rc = sqlite3_step(stmt);
                if (rc == SQLITE_ROW) {
                        hits++;
                }
                sqlite3_reset(stmt);
        }
        sqlite3_finalize(stmt);
        t3 = now();

        /* 3) range count, window up to 512 wide */
        sqlite3_prepare_v2(db, "SELECT COUNT(*) FROM t WHERE k BETWEEN ? AND ?",
                           -1, &stmt, NULL);
        for (i = 0; i < N / 4; i++) {
                long k1 = (long)(rng() % N + 1);
                sqlite3_bind_int64(stmt, 1, (sqlite3_int64)k1);
                sqlite3_bind_int64(stmt, 2, (sqlite3_int64)(k1 + (rng() % 512)));
                if (sqlite3_step(stmt) == SQLITE_ROW) {
                        rows += (unsigned long)sqlite3_column_int64(stmt, 0);
                }
                sqlite3_reset(stmt);
        }
        sqlite3_finalize(stmt);
        t4 = now();

        sqlite3_close(db);
        rm_db();

        printf("sequential insert      : %ld keys in %.3fs -> %.0f keys/s\n",
               (long)N, t1 - t0, N / (t1 - t0));
        printf("random point query     : %ld keys in %.3fs -> %.0f keys/s (hits %lu)\n",
               (long)N, t3 - t2, N / (t3 - t2), hits);
        printf("range query (<=512)    : %ld queries in %.3fs -> %.0f q/s (rows %lu)\n",
               (long)N / 4, t4 - t3, (N / 4) / (t4 - t3), rows);
        return 0;
}
