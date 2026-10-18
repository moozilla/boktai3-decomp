#include "global.h"
struct PositionNode {
    struct PositionNode *next;
    u16 unk4, flags;
    u8 pad8[4];
    u16 offset[3];
    u16 unk12;
    u16 base[3];
    u8 pad1A[10];
    u16 translated[3];
};
void PositionList_UpdateTranslatedCoordinates(struct PositionNode **first,
                                              struct PositionNode **second)
{
    struct PositionNode *next;
    struct PositionNode **saved;
    struct PositionNode *node;
    saved = second;
    next = *first;
    while (next) {
        node = next;
        next = next->next;
        node->translated[0] = node->offset[0] + node->base[0];
        node->translated[1] = node->offset[1] + node->base[1];
        node->translated[2] = node->offset[2] + node->base[2];
    }
    {
        struct PositionNode *node;
        struct PositionNode *next = *saved;
        while (next) {
            node = next;
            next = next->next;
            if (!(node->flags & 4)) {
                node->translated[0] = node->offset[0] + node->base[0];
                node->translated[1] = node->offset[1] + node->base[1];
                node->translated[2] = node->offset[2] + node->base[2];
            }
        }
    }
}
