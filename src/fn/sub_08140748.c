#include "global.h"
#include "player.h"

// fix.py best version (reprinted C)
// Timer-address constant reuse and mask-load order still differ.
s32 Math_Atan2(s32, s32);
static inline s32 Player_QuantizeTerrainFacing(u16 angle)
{
    return ((((angle + 0x60) >> 6) & 3) * 2) + 1;
}

s32 Player_BeginTerrainFacingAction(struct Player *p)
{
    u32 direction;
    u16 angle;
    u8 facing;

    if (p->actionMode == 5) {
        p->actionLatch464 = 1;
        p->actionLatch465 = 1;
    }
    p->actionActive489 = 1;
    p->directionFrame = 50;
    {
        u16 duration = 60;
        p->positionResetValue = duration;
    }
    p->contactSpecial = 0;
    p->terrainRecoveryCounter = 0;
    angle = Math_Atan2(p->position.x - p->terrainTarget->x, p->position.z - p->terrainTarget->z);
    direction = (angle + 96) >> 6;
    {
        s32 mask = 3;
        mask &= direction;
        mask <<= 1;
        mask += 1;
        facing = p->facing = mask;
        if ((u32)mask > 4) {
            p->heading = (8 - p->facing) >> 1;
            p->side = 1;
        } else {
            p->heading = facing >> 1;
            p->side = 0;
        }
    }
    p->contactActionTag = 0xE6;
    p->actionPhase488 = 0;
    return 10;
}
