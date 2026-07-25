#include "hashtable.h"

#include <stddef.h>

uint64_t hash_int(const void* key) {
        uint64_t hash = *(uint64_t*)(uintptr_t)key;

        hash ^= hash >> 30;
        hash *= SPLITMIX_64_CONST_1;
        hash ^= hash >> 27;
        hash *= SPLITMIX_64_CONST_2;
        hash ^= hash >> 31;

        return hash;
}
uint64_t hash_string(const void* key_str) {
        uint64_t hash = FNV_64_OFFSET;
        char* key = (char*)key_str;

        while (*key) {
                hash ^= (uint64_t)(unsigned char)(*key);
                hash *= FNV_64_PRIME;
                key++;
        }
        return hash;
}

uint64_t hash_ptr(const void* key) {
        uintptr_t ptr_address = (uintptr_t)key;
        const uintptr_t* ptr_ptr = &ptr_address;

        return hash_int(ptr_ptr);
}

HashTable* hash_table_create(int key_type, int value_type) {
        HashTable* hash_table = malloc(sizeof(HashTable));
        if (!hash_table) {
                return NULL;
        }

        hash_table->hash_key_type = key_type;
        hash_table->size = TABLE_SIZE;
        hash_table->used_bucket_count = 0;

        switch (key_type) {
                case HASH_TYPE_INT:
                        hash_table->hash_func = hash_int;
                        break;
                case HASH_TYPE_STRING:
                        hash_table->hash_func = hash_string;
                        break;
                case HASH_TYPE_PTR:
                        hash_table->hash_func = hash_ptr;
                        break;
                default:
                        free(hash_table);
                        return NULL;
        }

        switch (value_type) {
                case HASH_TYPE_INT:
                case HASH_TYPE_PTR:
                case HASH_TYPE_STRING:
                        hash_table->hash_value_type = value_type;
                        break;
                default:
                        free(hash_table);
                        return NULL;
        }

        hash_table->buckets = calloc(hash_table->size, sizeof(HashEntry*));
        if (hash_table->buckets == NULL) {
                free(hash_table);
                return NULL;
        }

        return hash_table;
}

int hash_table_put(HashTable* hash_table, void* key, void* value) {
        uint64_t hash_value = hash_table->hash_func(key);
        uint64_t bucket_idx = hash_value % hash_table->size;

        HashEntry* existing_hash_entry = hash_table->buckets[bucket_idx];
        int bucket_was_empty = (existing_hash_entry == NULL);
        int is_value_replaced = 0, chain_collisions_exceed_max = 0;
        size_t chain_collision_count = 0;
        if (bucket_was_empty) {
                hash_table->used_bucket_count++;
        }

        while (existing_hash_entry != NULL) {
                if (key_compare(existing_hash_entry->key, key,
                                hash_table->hash_key_type) == 1) {
                        value_destructor(existing_hash_entry->value);
                        existing_hash_entry->value = value;
                        is_value_replaced = 1;
                        break;
                }

                chain_collision_count++;
                existing_hash_entry = existing_hash_entry->next;
                if (chain_collision_count >= MAX_CHAIN_LENGTH) {
                        chain_collisions_exceed_max = 1;
                        break;
                }
        }

        if (!is_value_replaced) {
                HashEntry* hash_entry = malloc(sizeof(HashEntry));
                if (!hash_entry) {
                        if (bucket_was_empty) {
                                hash_table->used_bucket_count--;
                        }
                        return -1;
                }
                hash_entry->key = key;
                hash_entry->value = value;
                hash_entry->next = hash_table->buckets[bucket_idx];
                hash_table->buckets[bucket_idx] = hash_entry;
        }

        if ((float)hash_table->used_bucket_count / (float)hash_table->size >
                0.7f ||
            chain_collisions_exceed_max) {
                hash_table_resize(hash_table);
        }

        return 1;
}

void* hash_table_get(HashTable* hash_table, void* key) {
        uint64_t hash_value = hash_table->hash_func(key);
        uint64_t bucket_idx = hash_value % hash_table->size;

        HashEntry* existing_hash_entry = hash_table->buckets[bucket_idx];

        if (existing_hash_entry == NULL) {
                return NULL;
        }

        while (existing_hash_entry != NULL) {
                if (key_compare(existing_hash_entry->key, key,
                                hash_table->hash_key_type) == 1) {
                        return existing_hash_entry->value;
                }
                // deal with ptr type later...
                //
                // if (hash_table->hash_key_type == HASH_TYPE_PTR)
                existing_hash_entry = existing_hash_entry->next;
        }
        return NULL;
}

int key_compare(const void* key1, const void* key2, int key_type) {
        int key_match;
        key_match = 0;

        if (key_type == HASH_TYPE_INT) {
                if (*(uint64_t*)key1 == *(uint64_t*)key2) {
                        key_match = 1;
                }
        }
        if (key_type == HASH_TYPE_STRING) {
                if (strcmp((char*)key1, (char*)key2) == 0) {
                        key_match = 1;
                }
        }
        return key_match;
}

void key_destructor(void* key) {
        if (key == NULL) {
                return;
        }
        free(key);
}

void value_destructor(void* value) {
        if (value == NULL) {
                return;
        }
        free(value);
}

void hash_table_resize(HashTable* hash_table) {
        size_t bucket_counter = 0;
        size_t old_size = hash_table->size;
        HashEntry** old_buckets = hash_table->buckets;
        hash_table->size = hash_table->size * 1.5;
        hash_table->used_bucket_count = 0;
        HashEntry** new_buckets = calloc(hash_table->size, sizeof(HashEntry*));
        if (new_buckets == NULL) {
                hash_table->size = old_size;
                return;
        }
        hash_table->buckets = new_buckets;
        while (bucket_counter < old_size) {
                HashEntry* entry = old_buckets[bucket_counter];
                while (entry != NULL) {
                        hash_table_put(hash_table, entry->key, entry->value);
                        HashEntry* prev_entry = entry;
                        entry = entry->next;
                        free(prev_entry);
                }
                bucket_counter++;
        }
        free(old_buckets);
}

int hash_table_remove(HashTable* hash_table, void* key) {
        uint64_t hash_value = hash_table->hash_func(key);
        uint64_t hash_idx = hash_value % hash_table->size;
        HashEntry* target = hash_table->buckets[hash_idx];

        if (target == NULL) {
                return 0;
        }

        if (key_compare(target->key, key, hash_table->hash_key_type) == 1) {
                hash_table->buckets[hash_idx] = target->next;
                key_destructor(target->key);
                value_destructor(target->value);
                free(target);
                return 1;
        }

        HashEntry* previous = target;
        target = target->next;
        while (target != NULL) {
                if (key_compare(key, target->key, hash_table->hash_key_type) ==
                    1) {
                        HashEntry* next_node = target->next;
                        key_destructor(target->key);
                        value_destructor(target->value);
                        free(target);
                        previous->next = next_node;
                        return 1;
                }
                previous = target;
                target = target->next;
        }

        return -1;
}

void ht_print(HashTable* hash_table) {
        size_t count;
        count = 0;

        while (count < hash_table->size) {
                HashEntry* entry = hash_table->buckets[count];
                while (entry != NULL) {
                        switch (hash_table->hash_key_type) {
                                case HASH_TYPE_INT:
                                        printf("Key: %d ", *(int*)entry->key);
                                        break;
                                case HASH_TYPE_STRING:
                                        printf("Key: %s ", (char*)entry->key);
                                        break;
                                case HASH_TYPE_PTR:
                                        printf("Key: %p ", entry->key);
                                        break;
                                default:
                                        break;
                        }
                        switch (hash_table->hash_value_type) {
                                case HASH_TYPE_INT:
                                        printf("Value: %d\n",
                                               *(int*)entry->value);
                                        break;
                                case HASH_TYPE_STRING:
                                        printf("Value: %s\n",
                                               (char*)entry->value);
                                        break;
                                case HASH_TYPE_PTR:
                                        printf("Value: %p\n", entry->value);
                                        break;
                                default:
                                        break;
                        }
                        entry = entry->next;
                }
                ++count;
        }
}
