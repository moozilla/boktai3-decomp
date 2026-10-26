#include "global.h"
#include "render.h"
#define DirectionalSprite_InitEntry PlayerClone_InitSlot
#define gDirectionalSpritePool      gUnk_020005F8
struct DirectionalSprite {
    u8 unknown0[0x320];
    u8 foldedFacing;
    u8 facing;
    u8 mirrored;
    u8 unknown323[0x344 - 0x323];
};
struct DirectionalSpritePool {
    u8 unknown0[0x18];
    u32 activeMask;
    struct DirectionalSprite entries[3];
    u32 unknown9E8;
    s16 values[16];
    u8 copiedData[32];
};
extern struct DirectionalSpritePool *gUnk_020005F8;
void PlayerClone_InitSlot(void *, struct DirectionalSprite *, s32);
s32 DirectionalSpritePool_Init(struct DirectionalSpritePool *pool)
{
    struct DirectionalSprite *entry;
    s32 i;
    s16 *value;
    gUnk_020005F8 = pool;
    pool->activeMask = 0;
    entry = pool->entries;
    i = 0;
    do {
        PlayerClone_InitSlot(pool, entry, i);
        i++;
        entry++;
    } while (i <= 2);
    {
        u8 **palettes = &gPaletteBankData;
        u16 blank = 0x7FFF;
        i = 15;
        value = &pool->values[15];
        do {
            *value = blank;
            value--;
            i--;
        } while (i >= 0);
        CpuSet(*palettes + 0x3E80, pool->copiedData, 0x04000008);
    }
    return 0;
}
