#include "global.h"
struct Nd;
struct T;
struct Nd *HashTable_FindNode(struct T *t, u32 key);
void HashTable_RemoveKey(struct T *t, u32 key);
void HashNode_Init(struct Nd *n, u32 key, u32 value);
s32 HashTable_InsertNode(struct T *t, struct Nd *n);

s32 sub_08222504(struct T *t, u32 key, u32 newKey)
{
    struct Nd *n = HashTable_FindNode(t, key);
    if (n == NULL)
        return -1;
    HashTable_RemoveKey(t, key);
    HashNode_Init(n, newKey, ((u32 *)n)[1]);
    return HashTable_InsertNode(t, n);
}
