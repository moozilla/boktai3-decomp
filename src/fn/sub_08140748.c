
#ifdef NONMATCHING
// fuzzy: 84.6%
// foundry draft (worker best 84.6%); what differs is mostly register allocation and instruction order
#define Player_BeginTerrainFacingAction sub_08140748
#include "global.h"
#include "player.h"
s32 Math_Atan2(s32, s32);
s32 sub_08140748(struct Player *p)
{
    u8 direction;
    if (p->actionMode == 5) {
        p->actionLatch464 = 1;
        p->actionLatch465 = 1;
    }
    p->actionActive489 = 1;
    p->directionFrame = 50;
    p->positionResetValue = 60;
    p->contactSpecial = 0;
    p->terrainRecoveryCounter = 0;
    direction = (((u16)Math_Atan2(p->position.x - p->terrainTarget->x,
                                  p->position.z - p->terrainTarget->z) + 0x60) >> 6) & 3;
    direction = direction * 2 + 1;
    p->facing = direction;
    if (p->facing > 4) {
        p->heading = (8 - p->facing) >> 1;
        p->side = 1;
    } else {
        p->heading = direction >> 1;
        p->side = 0;
    }
    p->contactActionTag = 0xE6;
    p->actionPhase488 = 0;
    return 10;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08140748);
#endif
