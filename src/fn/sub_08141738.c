#define gDirectionSineTable                 gUnk_086149C4
#include "global.h"
#include "player.h"
extern const s16 gUnk_086149C4[];
static inline s32 ScaleCameraOffset(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}
void Player_BeginDirectionalCameraOffset(struct Player *p)
{
    if (p->cameraOffsetActive == 0) {
        s32 angle = ((p->coordinateAxis + 1) & 7) << 5;
        p->cameraOffsetX = -ScaleCameraOffset(gUnk_086149C4[(angle + 64) & 255] * 800);
        p->cameraOffsetY = 0;
        p->cameraOffsetZ = -ScaleCameraOffset(gUnk_086149C4[angle] * 800);
        p->cameraOffsetActive = 1;
    }
}
