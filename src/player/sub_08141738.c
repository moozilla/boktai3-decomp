#include "bosses/vanargand_projectile/vanargand_projectiles.h"
#define gDirectionSineTable gUnk_086149C4
#include "engine/shared/global.h"
#include "engine/shared/player.h"
extern const s16 gUnk_086149C4[];
void Player_BeginDirectionalCameraOffset(struct Player *p)
{
    if (p->cameraOffsetActive == 0) {
        s32 angle = ((p->coordinateAxis + 1) & 7) << 5;
        p->cameraOffsetX = -VanargandProjectile_Scale(gUnk_086149C4[(angle + 64) & 255] * 800);
        p->cameraOffsetY = 0;
        p->cameraOffsetZ = -VanargandProjectile_Scale(gUnk_086149C4[angle] * 800);
        p->cameraOffsetActive = 1;
    }
}
