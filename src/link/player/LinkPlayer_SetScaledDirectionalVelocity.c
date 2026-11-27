#include "bosses/vanargand/vanargand_projectiles.h"
#include "monsters/ghoul_disc.h"
#define gDirectionSine gUnk_086149C4
#include "global.h"
/* LinkPlayer_SetScaledDirectionalVelocity: same code as Player_SetDirectionalVelocity at another address (ported by relatives.py; provisional name) */
#include "player/player.h"
extern const s16 gUnk_086149C4[];
void LinkPlayer_SetScaledDirectionalVelocity(struct Player *p, s32 speed)
{
    s32 angle = ((p->facing + 5) & 7) << 5;
    p->velocityX = VanargandProjectile_Scale(gUnk_086149C4[(angle + 64) & 255] * speed);
    p->velocityZ = VanargandProjectile_Scale(gUnk_086149C4[angle] * speed);
    if (p->velocityScaleAxis == 1) {
        if (p->velocityZ < 0)
            p->velocityZ = Ghoul_QuarterDirection(p->velocityZ * 40);
        else
            p->velocityZ = Ghoul_QuarterDirection(p->velocityZ * 52);
    } else if (p->velocityScaleAxis == 2) {
        if (p->velocityX < 0)
            p->velocityX = Ghoul_QuarterDirection(p->velocityX * 40);
        else
            p->velocityX = Ghoul_QuarterDirection(p->velocityX * 52);
    }
}
