#include "global.h"
#include "player.h"

#ifdef NONMATCHING
// fuzzy: 90.7%
// Timer-address constant reuse and mask-load order still differ.
s32 Math_Atan2(s32, s32);
static inline s32 Player_QuantizeTerrainFacing(s32 angle)
{
    s32 direction = 3;
    direction &= ((u16)angle + 0x60) >> 6;
    return direction * 2 + 1;
}
s32 Player_BeginTerrainFacingAction(struct Player *p)
{
    s32 direction;
    if (p->actionMode == 5) {
        p->actionLatch464 = 1;
        p->actionLatch465 = 1;
    }
    p->actionActive489 = 1;
    p->directionFrame = 50;
    p->positionResetValue = 60;
    p->contactSpecial = 0;
    p->terrainRecoveryCounter = 0;
    direction = Player_QuantizeTerrainFacing(
        Math_Atan2(p->position.x - p->terrainTarget->x, p->position.z - p->terrainTarget->z));
    p->facing = direction;
    if (p->facing > 4) {
        p->heading = (8 - p->facing) >> 1;
        p->side = 1;
    } else {
        p->heading = (u8)direction >> 1;
        p->side = 0;
    }
    p->contactActionTag = 0xE6;
    p->actionPhase488 = 0;
    return 10;
}
#else
INCLUDE_ASM("asm/nonmatching", Player_BeginTerrainFacingAction);
#endif
