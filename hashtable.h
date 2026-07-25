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

typedef struct HashTable {
        HashEntry** buckets;
        size_t used_bucket_count;
        size_t size;
        int hash_key_type;
        int hash_value_type;
        bool is_downsizing;

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
} HashTableResult;

uint64_t hash_int(const void* key);
uint64_t hash_string(const void* key_str);
uint64_t hash_ptr(const void* key_ptr);

bool key_compare(const void* key1, const void* key2, int key_type);
void key_destructor(void* key);
void value_destructor(void* value);

// Returns pointer to the newly created hash_table instance.
// key_type and value_type must be valid hashtable types,
// as defined above. Invalid values will return NULL to the caller;
HashTable* hash_table_create(int key_type, int value_type);

HashTableResult hash_table_put(HashTable* hash_table, void* key, void* value);
void* hash_table_get(HashTable* hash_table, void* key);
bool key_value_compare(HashEntry* hash_entry, void* key, void* value,
                      int key_type, int value_type);

HashTableResult hash_table_remove(HashTable* hash_table, void* key);
HashTableResult remove_kvp(HashTable* hash_table, void* key);
HashTableResult rehash_for_resize(HashTable* hash_table, HashEntry** old_buckets,
                       size_t old_size);
void hash_table_resize(HashTable* hash_table);
void hash_table_downsize(HashTable* hash_table);
void hash_table_destroy(HashTable* hash_table);
// removes an item from the hash table.
// if the value attempting to remove doesnt exist (NULL), return 0.
// if it does exist and was successfully removed, return 1;
// all other results (likely errors) return -1.
void ht_print(HashTable* hash_table);
