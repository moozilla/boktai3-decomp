#include "global.h"
#include "player.h"
#include "engine.h"
s32 Mod(s32, s32);
s32 Div(s32, s32);
s32 Math_Atan2(s32, s32);
s32 Player_BeginTerrainRecovery_081408AC(struct Player *p)
{
    s32 x, z, angle, tx, tz;
    u32 direction;
    u8 facing;
    p->terrainRecoveryActive = 1;
    p->directionFrame = 50;
    p->contactSpecial = 0;
    p->terrainRecoveryCounter = 0;
    x = Mod(p->recoveryTerrainCell, *(s16 *)(gTerrainMap->cells + 4));
    z = Div(p->recoveryTerrainCell, *(s16 *)(gTerrainMap->cells + 4));
    x <<= 8;
    tx = p->terrainTarget->x;
    tx -= 128;
    x -= tx;
    z <<= 8;
    tz = p->terrainTarget->z;
    tz -= 128;
    z -= tz;
    angle = ((u16)Math_Atan2(x, z) + 0x60) >> 6;
    direction = 3;
    direction &= angle;
    direction <<= 1;
    direction += 1;
    p->facing = direction;
    facing = direction;
    if ((u32)direction > 4) {
        p->heading = (8 - p->facing) >> 1;
        p->side = 1;
    } else {
        p->heading = facing >> 1;
        p->side = 0;
    }
    p->contactActionTag = 0x191;
    p->terrainRecoveryPhase = 0;
    return 10;
}
