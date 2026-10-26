#include "global.h"
#include "ram.h"
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
// confidence=low: finds the pool's kind-1 item owned by owner, or NULL.
struct OwnedItem *sub_08120FF4(void *owner)
{
    struct OwnedItemPool *pool;
    s32 i;
    if (gActorManager == NULL)
        return NULL;
    pool = gActorManager;
    for (i = 0; i < 1; i++) {
        if (pool->items[i].kind == 1 && pool->items[i].owner == owner)
            return &pool->items[i];
    }
    return NULL;
}
