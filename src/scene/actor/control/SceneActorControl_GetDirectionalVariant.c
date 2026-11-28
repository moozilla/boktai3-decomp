#include "global.h"

/* Splits a direction into one of five animation variants and whether it is mirrored. */
void SceneActorControl_GetDirectionalVariant(u16 direction, s32 *variant, s32 *mirrored)
{
    s32 sector = ((direction + 0x70) >> 5) & 7;
    if (sector > 4) {
        *variant = 8 - sector;
        *mirrored = 1;
    } else {
        *variant = sector;
        *mirrored = 0;
    }
}
