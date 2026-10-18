#define gSpriteOamQueueCount  gUnk_03004798
#define gSpriteOamCount       gUnk_0300479C
#define gSpriteOamAffineCount gUnk_03004BA0
#define gSpriteOamQueueHeads  gUnk_03004790
#define gSpriteOamBuffer      gUnk_03004390
#include "global.h"
struct SpriteOamEntry {
    u32 attributes;
    u32 unknown4;
};
extern u32 gUnk_03004798, gUnk_0300479C, gUnk_03004BA0;
extern u16 gUnk_03004790[4];
extern struct SpriteOamEntry gUnk_03004390[128];
void SpriteOam_ResetQueue(s32 clear)
{
    u16 *p, *base;
    u32 *count, *affine;
    gUnk_03004798 = 0;
    count = &gUnk_0300479C;
    affine = &gUnk_03004BA0;
    base = gUnk_03004790;
    {
        u16 zero = 0;
        p = base + 3;
        do {
            *p-- = zero;
        } while ((s32)p >= (s32)base);
    }
    *count = 0;
    *affine = 0;
    if (clear) {
        struct SpriteOamEntry *entry = gUnk_03004390;
        u32 hidden = 0x200;
        s32 i = 127;
        do {
            entry->attributes = hidden;
            entry++;
        } while (--i >= 0);
    }
}
