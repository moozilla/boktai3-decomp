#include "global.h"
#define CallbackPool_RemoveSmallSprites sub_08115FAC
#define gSmallCallbackPool              gUnk_020001E8
struct SmallCallbackEntry {
    u8 pad0[0x1C];
    u8 sprite[4];
    u8 spriteActive;
    u8 pad21[0x8F];
    u8 active;
    u8 padB1[3];
    void *callback;
};
struct SmallCallbackPool {
    u8 pad0[0x18];
    struct SmallCallbackEntry entries[16];
};
extern struct SmallCallbackPool *gUnk_020001E8;
void Sprite_Remove(void *);
s32 sub_08115FAC(struct SmallCallbackPool *pool)
{
    s32 i;
    for (i = 0; i <= 15; i++) {
        if (pool->entries[i].spriteActive) {
            Sprite_Remove(pool->entries[i].sprite);
        }
    }
    return (s32)(gUnk_020001E8 = 0);
}
