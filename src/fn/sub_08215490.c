#define gSpriteOamQueueCount  gOamCount
#define gSpriteOamCount       gPendingOamCount
#define gSpriteOamAffineCount gAffineRequestCount
#define gSpriteOamQueueHeads  gOamPriorityCounts
#define gSpriteOamBuffer      gUnk_03004390
#include "global.h"
#include "render.h"
struct SpriteOamEntry {
    u32 attributes;
    u32 unknown4;
};
extern struct SpriteOamEntry gUnk_03004390[128];
void SpriteOam_ResetQueue(s32 clear)
{
    u16 *p, *base;
    u32 *count, *affine;
    gOamCount = 0;
    count = &gPendingOamCount;
    affine = &gAffineRequestCount;
    base = gOamPriorityCounts;
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
