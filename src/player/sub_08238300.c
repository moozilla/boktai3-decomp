#include "bosses/vanargand_projectile/vanargand_projectiles.h"
#include "monsters/ghoul/ghoul_disc.h"
#define gDirectionSine gUnk_086149C4
#include "engine/shared/global.h"
/* Player_SetDirectionalVelocity_08238300: same code as Player_SetDirectionalVelocity at another address (ported by relatives.py; provisional name) */
#include "engine/shared/player.h"
extern const s16 gUnk_086149C4[];
void Player_SetDirectionalVelocity_08238300(struct Player *p, s32 speed)
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
