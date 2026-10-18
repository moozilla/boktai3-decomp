#include "global.h"
#include "level_advance_display.h"
struct BriefSpriteEffect {
    u8 unknown0[0x744];
    void *resource;
    u32 flags;
    u8 unknown74C[0x14];
    union LevelAdvancePosition position;
    u8 unknown768[8];
    u8 active, tick, firstFrame;
};
void SpriteEffect_StartBriefAnimation(struct BriefSpriteEffect *p,
                                      union LevelAdvancePosition *position, s32 alternate)
{
    struct LevelAdvanceParticle *sprite = (struct LevelAdvanceParticle *)&p->flags;
    if (alternate)
        p->firstFrame = 0x48;
    else
        p->firstFrame = 4;
    SpriteDrawable_SelectTileFrame(sprite, p->resource, p->firstFrame);
    sprite->position = *position;
    sprite->flags &= ~1;
    p->active = 1;
    p->tick = 0;
}
