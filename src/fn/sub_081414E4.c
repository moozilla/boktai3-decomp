#define Player_ClampCameraOffsets sub_081414E4
#include "global.h"
#include "player.h"
void sub_081414E4(struct Player *p)
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
