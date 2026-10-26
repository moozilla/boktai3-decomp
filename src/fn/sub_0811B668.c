#include "global.h"
#include "ram.h"
#define gCategoryLists gSpatialIndex
struct CategoryListNode {
    u32 value;
    struct CategoryListNode *previous;
    struct CategoryListNode *next;
};
struct CategoryListPair {
    struct CategoryListNode *flag8Nodes, *flag16Nodes;
    u32 unknown8, unknownC;
};
struct CategoryLists {
    u8 task[0x18];
    struct CategoryListPair lists[10];
};
void Heap_Free(void *);
s32 CategoryLists_Destroy(struct CategoryLists *manager)
{
    s32 i = 0;
    do {
        struct CategoryListNode *node = manager->lists[i].flag16Nodes;
        while (node) {
            struct CategoryListNode *next = node->next;
            Heap_Free(node);
            node = next;
        }
        node = manager->lists[i].flag8Nodes;
        while (node) {
            struct CategoryListNode *next = node->next;
            Heap_Free(node);
            node = next;
        }
        i++;
    } while (i <= 9);
    gSpatialIndex = 0;
    return 0;
}
