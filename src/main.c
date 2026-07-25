#include <stdio.h>
#include "hashtable.h"


int main() {

    
    HashTable *hash_table = hash_table_create(HASH_TYPE_STRING, HASH_TYPE_STRING);
    if (hash_table == NULL) {
        printf("hash_table null\n");
    }
    char *name1 = "Brendan";
    char *val1 = "The Best";
    char *name2 = "Tyler";
    char *val2 = "Smith";
    char *name3 = "John";
    char *val3 = "Slade";
    printf("Created hash table\n");
    printf("Set fn ptrs\n");
    
    printf("Data generated/allocated\n");
    hash_table_put(hash_table, (void*)name1, (void*)val1);
    hash_table_put(hash_table, (void*)name2, (void*)val2);
    hash_table_put(hash_table, (void*)name3, (void*)val3);

    printf("data added to hashtable\n");
    void* val1_hashed = hash_table_get(hash_table, (void*)name1);
    printf("Val1 retrieved: %s\n", (char*)val1_hashed);
    hash_table_resize_test(hash_table);
    printf("Resized.\n");
    void* val2_hashed_resized = hash_table_get(hash_table, (void*)name1);
    printf("Val2 retrieved: %s\n", (char*)val2_hashed_resized);
    return 0;
}
