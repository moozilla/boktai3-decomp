#include "global.h"
struct HashEntry {
    u32 key;
    void *value;
    struct HashEntry *next;
};
struct HashTable {
    u32 bucketCount;
    struct HashEntry **buckets;
    void *hash;
};
u32 sub_08249240(struct HashTable *, u32, void *);
static inline u8 HashEntry_HasKey(struct HashEntry *entry, u32 key)
{
    u32 value = entry->key;
    u8 result = 0;
    if (value == key)
        result = 1;
    return result;
}
s32 HashTable_RemoveKey(struct HashTable *table, u32 key)
{
    struct HashEntry **buckets = table->buckets;
    struct HashEntry *previous;
    struct HashEntry *entry;
    void *hash;
    u32 index;
    if (!buckets || !(hash = table->hash))
        return -1;
    index = sub_08249240(table, key, hash);
    previous = 0;
    entry = buckets[index];
    while (entry) {
        if (HashEntry_HasKey(entry, key)) {
            if (previous)
                previous->next = entry->next;
            else
                buckets[index] = entry->next;
            return 0;
        }
        previous = entry;
        entry = entry->next;
    }
    return -2;
}
