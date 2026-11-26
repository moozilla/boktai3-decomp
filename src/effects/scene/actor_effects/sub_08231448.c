#include "global.h"
struct ActiveListNode {
    u8 active;
    u8 unknown01[0x12f];
    struct ActiveListNode *previous;
    struct ActiveListNode *next;
};
struct ActiveList {
    u8 unknown00[0x20];
    struct ActiveListNode *head;
};
s32 ActiveList_PrependNode(struct ActiveList *list, struct ActiveListNode *node)
{
    if (node->active)
        return -1;
    node->previous = 0;
    node->next = list->head;
    if (node->next)
        node->next->previous = node;
    list->head = node;
    node->active = 1;
    return 0;
}
