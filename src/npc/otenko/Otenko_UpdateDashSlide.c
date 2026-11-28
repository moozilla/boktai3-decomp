#include "npc/otenko/otenko_actor.h"
#include "ui/hud/scene_indicator.h"
#include "engine/math/math_shift.h"

extern const s16 gUnk_086149C4[];

void Otenko_UpdateDashSlide(void *ctx, struct OtenkoActor *npc)
{
    if (npc->animationEntered) {
        npc->animationEntered = 0;
        npc->done = 0;
        Actor_PlayAnimationEntry(npc, 9, 2, 0, 4);
    }
    if ((u8)npc->animationCallback(npc) != 1) {
        if (npc->speed == 0) {
            npc->done = 1;
        } else {
            struct ContactPosition *velocity = &npc->body.motion;
            u8 angle = npc->targetAngle;
            s32 speed = npc->speed;
            velocity->x = Math_ShiftTowardZero(speed * gUnk_086149C4[(angle + 64) & 255], 12);
            velocity->y = 0;
            velocity->z = Math_ShiftTowardZero(speed * gUnk_086149C4[angle], 12);
            npc->speed = (u32)npc->speed * 3 / 4;
        }
    }
}
