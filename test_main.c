#include "unity.h"
#include "hashtable.h"

void setUp(void) {}
void tearDown(void) {}

void test_hash_table_create_returns_valid_table(void) {
        HashTable* ht = hash_table_create(HASH_TYPE_STRING, HASH_TYPE_STRING);
        TEST_ASSERT_NOT_NULL(ht);
        TEST_ASSERT_EQUAL(0, ht->used_bucket_count);
        hash_table_destroy(ht);
}

void test_hash_table_put_and_get_roundtrip(void) {
        HashTable* ht = hash_table_create(HASH_TYPE_STRING, HASH_TYPE_STRING);
        char* key = strdup("hello");
        char* value = strdup("world");
        hash_table_put(ht, key, value);
        TEST_ASSERT_EQUAL_STRING("world", (char*)hash_table_get(ht, "hello"));
        hash_table_destroy(ht);
}

void test_hash_table_put_get_delete_get_roundtrip(void) {
        HashTable* ht = hash_table_create(HASH_TYPE_STRING, HASH_TYPE_STRING);
        char* key = strdup("hello");
        char* value = strdup("world");
        hash_table_put(ht, key, value);
        TEST_ASSERT_EQUAL_STRING("world", (char*)hash_table_get(ht, "hello"));
        hash_table_remove(ht, "hello");
        TEST_ASSERT_NULL(hash_table_get(ht, "hello"));
        hash_table_destroy(ht);
}

void test_hash_table_resize(void) { 
    HashTable* ht = hash_table_create(HASH_TYPE_STRING, HASH_TYPE_STRING);
    char* key = strdup("hello");
    char* value = strdup("world");
    hash_table_put(ht, key, value);
    hash_table_resize(ht);
    TEST_ASSERT_EQUAL_STRING("world", (char*) hash_table_get(ht, "hello"));
    hash_table_destroy(ht);
}
int main(void) {
        UNITY_BEGIN();
        RUN_TEST(test_hash_table_create_returns_valid_table);
        RUN_TEST(test_hash_table_put_and_get_roundtrip);
        RUN_TEST(test_hash_table_put_get_delete_get_roundtrip);
        return UNITY_END();
}
