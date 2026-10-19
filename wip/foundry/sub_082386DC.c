#define SpriteEffect_AdvanceBriefAnimation sub_082386DC
#include "global.h"
#include "level_advance_display.h"
struct BriefSpriteEffect {
    u8 unknown0[0x744];
    void *resource;
    u32 flags;
    u8 unknown74C[0x24];
    u8 active, tick, firstFrame;
};
void sub_082386DC(struct BriefSpriteEffect *p)
{
    if (p->active) {
        u16 tick = p->tick + 1;
        p->tick = tick;
        if (p->tick > 5) {
            p->flags |= 1;
            p->active = 0;
        } else {
            sub_08217EEC((struct LevelAdvanceParticle *)&p->flags, p->resource, ((u8)tick >> 2) + p->firstFrame);
        }
    }
}
