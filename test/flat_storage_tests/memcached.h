/* -*- Mode: C; tab-width: 4; c-basic-offset: 4; indent-tabs-mode: nil -*- */

#if !defined(_dummy_memcached_h_)
#define _dummy_memcached_h_

#include "generic.h"

#include <netinet/in.h>
#include <sys/time.h>

#define V_LPRINTF(min_verbosity, string, ...)                           \
    if (verbose >= min_verbosity) {                                     \
        fprintf(stdout, "  *%s",                                        \
                &indent_str[sizeof(indent_str) - min_verbosity - 1]);   \
        fprintf(stdout, string, ##__VA_ARGS__);                         \
    }                                                                   \

#define V_PRINTF(min_verbosity, string, ...) if (verbose >= min_verbosity) fprintf(stdout, string, ##__VA_ARGS__)
#define V_FLUSH(min_verbosity) if (verbose >= min_verbosity) fflush(stdout)

#if defined(DEBUG)
#define TASSERT(expr, ...) if (! (expr)) { if (verbose) { printf("assertion failed(%d): %s", __LINE__, #expr); printf("\n" __VA_ARGS__); } assert(0); }
#else
#define TASSERT(expr, ...) if (! (expr)) { if (verbose) { printf("assertion failed(%d): %s", __LINE__, #expr); printf("\n" __VA_ARGS__); } return 1; }
#endif /* #if defined(DEBUG) */


extern const char indent_str[257];


#if defined(INIT_TEST)
#include "init_test.h"
#endif /* #if defined(INIT_TEST) */
#if defined(INLINE_TEST)
#include "inline_test.h"
#endif /* #if defined(INLINE_TEST) */
#if defined(ALLOC_DEALLOC_TEST)
#include "alloc_dealloc_test.h"
#endif /* #if defined(ALLOC_DEALLOC_TEST) */
#if defined(ITEM_WALK_TEST)
#include "item_walk_test.h"
#endif /* #if defined(ITEM_WALK_TEST) */
#if defined(PAGING_TEST)
#include "paging_test.h"
#endif /* #if defined(PAGING_TEST) */
#if defined(ASSOC_TEST)
#include "assoc_test.h"
#endif /* #if defined(ASSOC_TEST) */
#if defined(ALLOC_LARGE_LRU_EVICT_TEST)
#include "alloc_large_lru_evict_test.h"
#endif /* #if defined(ALLOC_LARGE_LRU_EVICT_TEST) */
#if defined(ALLOC_SMALL_LRU_EVICT_TEST)
#include "alloc_small_lru_evict_test.h"
#endif /* #if defined(ALLOC_SMALL_LRU_EVICT_TEST) */
#if defined(COMPLEX_ALLOC_TEST)
#include "complex_alloc_test.h"
#endif /* #if defined(COMPLEX_ALLOC_TEST) */

#if !defined(MAX_ITEM_SIZE)
#define MAX_ITEM_SIZE (1024 * 1024)
#endif /* #if !defined(MAX_ITEM_SIZE) */

#if !defined(KEY_MAX_LENGTH)
#define KEY_MAX_LENGTH 255
#endif /* #if !defined(KEY_MAX_LENGTH) */

/*
 * We only reposition items in the LRU queue if they haven't been repositioned
 * in this many seconds. That saves us from churning on frequently-accessed
 * items.
 */
#if !defined(ITEM_UPDATE_INTERVAL)
#define ITEM_UPDATE_INTERVAL 60
#endif /* #if !defined(ITEM_UPDATE_INTERVAL) */

#if !defined(MAX_KEYS)
#define MAX_KEYS (16 * 1024)
#endif /* #if !defined(MAX_KEYS) */

#if !defined(item_get_notedeleted)
#define item_get_notedeleted do_item_get_notedeleted
#endif /* #if !defined(item_get_notedeleted) */

#if !defined(stats_prefix_record_removal)
#define stats_prefix_record_removal(a, b, c, d, e) ;
#endif /* #if !defined(stats_prefix_record_removal) */

#if !defined(stats_evict)
#define stats_evict(a) ;
#endif /* #if !defined(stats_evict) */

#if !defined(stats_expire)
#define stats_expire(a) ;
#endif /* #if !defined(stats_expire) */


#if !defined(TOTAL_MEMORY)
#define TOTAL_MEMORY (4 * 1024 * 1024)
#endif /* #if !defined(TOTAL_MEMORY) */


#if !defined(FLAGS)
#define FLAGS   0xdeadbeef
#endif /* #if !defined(FLAGS) */


#if !defined(KEY)
#define KEY     "abcde"
#endif /* #if !defined(KEY) */


#define ITEM_CACHEDUMP_LIMIT (2 * 1024 * 1024)
#define ITEM_STATS_SIZES     (2 * 1024 * 1024)


#include "items.h"
#include "conn_buffer.h"


typedef struct conn_s conn;
struct conn_s {
    conn_buffer_group_t* cbg;

    bool binary;

    struct iovec* riov;        /* read iov */
    size_t riov_size;          /* number of read iovs allocated */
    size_t riov_curr;          /* current read iov being sent */
    size_t riov_left;          /* number of read iovs left to send */

    char   crlf[2];   /* used to receive cr-lfs from the ascii protocol. */
};

typedef struct stats_s       stats_t;
struct stats_s {
    unsigned int  curr_items;
    unsigned int  total_items;
    uint64_t      item_storage_allocated;
    uint64_t      item_total_size;
    unsigned int  curr_conns;
    unsigned int  total_conns;
    unsigned int  conn_structs;
    uint64_t      get_cmds;
    uint64_t      set_cmds;
    uint64_t      get_hits;
    uint64_t      get_misses;
    uint64_t      evictions;
    time_t        started;          /* when the process was started */
    uint64_t      bytes_read;
    uint64_t      bytes_written;

#define MEMORY_POOL(pool_enum, pool_counter, pool_string) uint64_t pool_counter;
#include "memory_pool_classes.h"

    struct {
#define MEMORY_POOL(pool_enum, pool_counter, pool_string) uint64_t pool_counter;
#include "memory_pool_classes.h"
    } mp_bytecount_errors_realloc_split;

    struct {
#define MEMORY_POOL(pool_enum, pool_counter, pool_string) uint64_t pool_counter;
#include "memory_pool_classes.h"
    } mp_bytecount_errors_free_split;
};

typedef struct settings_s    settings_t;
struct settings_s {
    size_t maxbytes;
    int maxconns;
    int port;
    int udpport;
    int binary_port;
    int binary_udpport;
    struct in_addr interf;
    int verbose;
    rel_time_t oldest_live; /* ignore existing items older than this */
    bool managed;          /* if 1, a tracker manages virtual buckets */
    int evict_to_free;
    char *socketpath;   /* path to unix socket if using local socket */
    double factor;          /* chunk size growth factor */
    int chunk_size;
    int num_threads;        /* number of libevent threads to run */
    char prefix_delimiter;  /* character that marks a key prefix (for stats) */
    int detail_enabled;     /* nonzero if we're collecting detailed stats */
};


extern settings_t settings;
extern rel_time_t current_time;
extern stats_t stats;

static inline int add_iov(conn* c, const void* ptr, const size_t size, bool is_first) { return 0; }


typedef struct {
  const item* it;
} find_in_lru_context_t;

typedef bool (*find_in_lru_funcptr_t) (const item* const item_to_be_tested,
                                       find_in_lru_context_t context);


extern bool find_in_lru_by_item_comparator(const item* item_to_be_tested, find_in_lru_context_t context);
extern const item* find_in_lru_by_funcptr(find_in_lru_funcptr_t comparator,
                                          find_in_lru_context_t context);
extern int check_lru_order(const item* item1, const item* item2);
extern int make_random_key(char* key, size_t key_size, bool max);

static inline const item* find_in_lru_by_item(const item* item_to_be_found) {
  find_in_lru_context_t temp;
  temp.it = item_to_be_found;
  return find_in_lru_by_funcptr(find_in_lru_by_item_comparator,
                                temp);
}
size_t append_to_buffer(char* const buffer_start,
                        const size_t buffer_size,
                        const size_t buffer_off,
                        const size_t reserved,
                        const char* fmt,
                        ...);


/* declaration of tests. */
typedef int (*tester_f)(int);
typedef struct {
    tester_f func;
    int is_fast;
} tester_info_t;

extern bool fa_freelist_check(const chunk_type_t ctype);
extern bool lru_check(void);
extern bool item_chunk_check(const item* it);
extern int verify_key(const item* it, const char* key);

#define STATS_LOCK() ;
#define STATS_UNLOCK() ;

#include "memory_pool.h"

#endif /* #if !defined(_dummy_memcached_h_) */
