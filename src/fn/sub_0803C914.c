#include "global.h"
struct Item {
    u8 pad[0x30];
    int id;
};
void *ScenePool_PackResourceKey(int);
void HashNode_Init(void *, void *, void *);
void HashTable_InsertNode(void *, void *);
void ScenePool_RegisterEntry(u8 *p, struct Item *item)
{
    void *resource = ScenePool_PackResourceKey(item->id);
    HashNode_Init(item, resource, item);
    HashTable_InsertNode(p + 0x2354, item);
}
