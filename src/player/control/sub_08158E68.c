#include "global.h"
#include "engine/engine.h"
#include "player/player.h"

s32 sub_08158E68(s32 i)
{
    struct Player *player = gPlayers[i];
    u8 *source = player->followTarget;
    s32 x, y;
    if (source != NULL)
        return *(u16 *)(source + 6);
    x = player->position.x >> 8;
    y = player->position.z >> 8;
    if (x < 0 || y < 0 || x >= gTerrainWidth || y >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[y] + x;
}
