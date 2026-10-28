#include "global.h"
#include "scene_pool.h"
#include "background_controller.h"
#include "render.h"
struct ScanlineScroll {
    u8 unknown0[0x1848];
    u16 scroll;
    u16 unknown184a;
    u8 layer, moving;
    s16 lastMapX;
    s16 remaining;
    u16 unknown1852;
    s32 speed;
    u16 offsets[160];
    u16 phases[160];
    u16 speedTop, speedBottom;
    u16 unknown1adc;
    u16 split;
};
static inline u8 IsPlayerScrollBlocked(void)
{
    if (gParticipantActors[0]) {
        u32 flags = gParticipantActors[0]->flags;
        if (flags & 5)
            return 1;
    }
    return 0;
}

void ScanlineScroll_AdvancePhases(struct ScanlineScroll *);
void ScanlineEffect_SetSource(s32, s32, void *);
void ScanlineScroll_Update(struct ScanlineScroll *p)
{
    if (p->lastMapX != gBgRecords[3].x)
        BgGfx_LoadTilemapSimple(3, gBgRecords[3].x >> 3, 0, 0, 0);
    p->lastMapX = gBgRecords[3].x;
    ScanlineScroll_AdvancePhases(p);
    ScanlineEffect_SetSource(3, 0, p->offsets);
    if (p->moving && !IsPlayerScrollBlocked()) {
        p->scroll += p->speed;
        if ((p->scroll >> 7) > 256)
            p->scroll = 0x8000;
        p->remaining--;
        if (p->remaining <= 0)
            p->moving = 0;
    }
    gBgRecords[p->layer].x = p->scroll >> 7;
}
