#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define TABLE_SIZE 1000
#define TABLE_SIZE_SCALE_FACTOR 1.5f
#define TARGET_DOWNSIZE_RATIO 0.45f
#define MAX_CHAIN_LENGTH 5
#define HT_FILL_RATIO 0.7f

#define HASH_TYPE_INT 1
#define HASH_TYPE_STRING 2
#define HASH_TYPE_PTR 3

#define FNV_64_OFFSET 0xCBF29CE484222325ULL
#define FNV_64_PRIME 0x00000100000001B3ULL

#define SPLITMIX_64_CONST_1 0xff51afd7ed558ccdUL
#define SPLITMIX_64_CONST_2 0x94d049bb133111ebULL

typedef struct HashEntry {
        void* key;
        void* value;
        struct HashEntry* next;
} HashEntry;

typedef struct HashTableStats {
        size_t used_bucket_count;
        size_t total_entries;
        float current_load;
} HashTableStats;

typedef struct HashTable {
        HashEntry** buckets;
        HashTableStats* stats;
        size_t size;
        int hash_key_type;
        int hash_value_type;
        bool is_downsizing;
        bool TESTING_COLLISION_FLAG;

        uint64_t (*hash_func)(const void*);
        int (*key_compare)(const void* key1, const void* key2, int key_type);
        void (*key_destructor)(void* key);
        void (*value_destructor)(void* value);
} HashTable;

typedef enum {
        HT_OK = 0,
        HT_ERR_ALLOC_FAILED,
        HT_ERR_KEY_NOT_FOUND,
        HT_ERR_INVALID_TYPE,
        HT_ERR_INVALID_ARGUMENT,
} HashTableResult;

HashTable* hash_table_create(int key_type, int value_type);
HashTableResult hash_table_put(HashTable* hash_table, void* key, void* value);
void* hash_table_get(HashTable* hash_table, void* key);
HashTableResult hash_table_remove(HashTable* hash_table, void* key);
bool hash_table_key_exists(HashTable* hash_table, void* key);

HashTableStats* get_statistics(HashTable* hash_table);
size_t get_total_size(HashTable* hash_table);

void hash_table_destroy(HashTable* hash_table);
void ht_print(HashTable* hash_table);
void hash_table_resize_test(HashTable* hash_table);
uint64_t test_hash_func_string(void* key);
uint64_t test_hash_always_zero(const void* key);
void test_rehash(HashTable* hash_table);
