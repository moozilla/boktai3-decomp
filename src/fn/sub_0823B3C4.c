#include "global.h"
/* Player_ClampCameraOffsets_0823B3C4: same code as Player_ClampCameraOffsets at another address (ported by relatives.py; provisional name) */
#define Player_ClampCameraOffsets_0823B3C4 sub_0823B3C4
#include "player.h"
void Player_ClampCameraOffsets_0823B3C4(struct Player *p)
{
    s32 correction = p->cameraOffsetY >> 1;
    s32 a = p->cameraOffsetX - correction;
    s32 b = p->cameraOffsetZ - correction;
    s32 difference = b - a;

    if (difference > 800) {
        correction = (difference - 800) >> 1;
        p->cameraOffsetX += correction;
        p->cameraOffsetZ -= correction;
    } else if (difference < -800) {
        correction = (-800 - difference) >> 1;
        p->cameraOffsetX -= correction;
        p->cameraOffsetZ += correction;
    }
    difference = b + a;
    if (difference > 800) {
        correction = (difference - 800) >> 1;
        p->cameraOffsetX -= correction;
        p->cameraOffsetZ -= correction;
    }
    if (difference < -800) {
        correction = (-800 - difference) >> 1;
        p->cameraOffsetX += correction;
        p->cameraOffsetZ += correction;
    }
}
