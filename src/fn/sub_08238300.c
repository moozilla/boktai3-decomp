#define gDirectionSine gUnk_086149C4
#include "global.h"
/* Player_SetDirectionalVelocity_08238300: same code as Player_SetDirectionalVelocity at another address (ported by relatives.py; provisional name) */
#define Player_SetDirectionalVelocity_08238300 sub_08238300
#include "player.h"
extern const s16 gUnk_086149C4[];
static inline s32 scale12(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -(-v >> 12);
}
static inline s32 scale6(s32 v)
{
    if (v >= 0)
        return v >> 6;
    return -(-v >> 6);
}
void Player_SetDirectionalVelocity_08238300(struct Player *p, s32 speed)
{
    s32 angle = ((p->facing + 5) & 7) << 5;
    p->velocityX = scale12(gUnk_086149C4[(angle + 64) & 255] * speed);
    p->velocityZ = scale12(gUnk_086149C4[angle] * speed);
    if (p->velocityScaleAxis == 1) {
        if (p->velocityZ < 0)
            p->velocityZ = scale6(p->velocityZ * 40);
        else
            p->velocityZ = scale6(p->velocityZ * 52);
    } else if (p->velocityScaleAxis == 2) {
        if (p->velocityX < 0)
            p->velocityX = scale6(p->velocityX * 40);
        else
            p->velocityX = scale6(p->velocityX * 52);
    }
}
