#include "global.h"
/* Player_BeginTerrainRecovery_0823BA78: same code as Player_BeginTerrainRecovery at another address (ported by relatives.py; provisional name) */
#include "player_core.h"
s32 Player_BeginTerrainRecovery_0823BA78(struct Player *p)
{
    u32 direction;
    u16 angle;
    u8 facing;

    p->terrainClampBlocked = 1;
    p->directionFrame = 50;
    Player_SetPositionResetDuration(p, 60);
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
    p->contactActionTag = 401;
    p->terrainClampApplied = 0;
    return 50;
}
