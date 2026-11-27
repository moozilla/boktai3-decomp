#include "npc/otenko/otenko_actor.h"
#include "ui/hud/scene_indicator.h"

extern const s16 gUnk_086149C4[];
static inline s32 trunc12(s32 x)
{
    return x >= 0 ? x >> 12 : -((-x) >> 12);
}
void Otenko_UpdateDashSlide(void *ctx, struct OtenkoActor *npc)
{
    if (npc->animationEntered) {
        npc->animationEntered = 0;
        npc->done = 0;
        Actor_PlayAnimationEntry(npc, 9, 2, 0, 4);
    }
    if ((u8)npc->animationCallback(npc) != 1) {
        u32 speed = npc->speed;
        if (speed == 0) {
            npc->done = 1;
            return;
        }
        {
            struct ContactPosition *vel = &npc->body.motion;
            u8 angle = npc->targetAngle;
            u32 spd = speed;
            vel->x = trunc12(spd * gUnk_086149C4[(angle + 0x40) & 0xff]);
            vel->y = 0;
            vel->z = trunc12(spd * gUnk_086149C4[angle]);
        }
        npc->speed = (u32)npc->speed * 3 / 4;
    }
}
