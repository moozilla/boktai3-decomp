#include "global.h"
#include "engine/math/math_shift.h"
#include "player/player.h"

extern const s16 gUnk_086149C4[];

/* LinkPlayer_SetScaledDirectionalVelocity: same code as Player_SetDirectionalVelocity at another address (ported by relatives.py; provisional name) */
void LinkPlayer_SetScaledDirectionalVelocity(struct Player *p, s32 speed)
{
    s32 angle = ((p->facing + 5) & 7) << 5;

    p->velocityX = Math_ShiftTowardZero(gUnk_086149C4[(angle + 64) & 255] * speed, 12);
    p->velocityZ = Math_ShiftTowardZero(gUnk_086149C4[angle] * speed, 12);
    if (p->velocityScaleAxis == 1) {
        if (p->velocityZ < 0)
            p->velocityZ = Math_ShiftTowardZero(p->velocityZ * 40, 6);
        else
            p->velocityZ = Math_ShiftTowardZero(p->velocityZ * 52, 6);
    } else if (p->velocityScaleAxis == 2) {
        if (p->velocityX < 0)
            p->velocityX = Math_ShiftTowardZero(p->velocityX * 40, 6);
        else
            p->velocityX = Math_ShiftTowardZero(p->velocityX * 52, 6);
    }
}
