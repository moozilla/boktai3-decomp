#include "global.h"
struct OwnedItem {
    u8 pad0[0x8B4];
    u32 kind;
    u8 pad8B8[4];
    void *owner;
    u8 pad8C0[0x908 - 0x8C0];
};
struct OwnedItemPool {
    u8 pad0[0x18];
    struct OwnedItem items[1];
};
extern struct OwnedItemPool *gUnk_0200020C;
// confidence=low: finds the pool's kind-1 item owned by owner, or NULL.
struct OwnedItem *sub_08120FF4(void *owner)
{
    struct OwnedItemPool *pool;
    s32 i;
    if (gUnk_0200020C == NULL)
        return NULL;
    pool = gUnk_0200020C;
    for (i = 0; i < 1; i++) {
        if (pool->items[i].kind == 1 && pool->items[i].owner == owner)
            return &pool->items[i];
    }
    return NULL;
}
