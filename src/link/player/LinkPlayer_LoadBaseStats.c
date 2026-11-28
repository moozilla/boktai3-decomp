#include "global.h"
#include "player/player.h"

void LinkSession_GetSlotValues(u32, s32 *, s32 *, s32 *, s32 *);

void LinkPlayer_LoadBaseStats(struct Player *p)
{
    s32 values[4];

    LinkSession_GetSlotValues(((const u8 *)p->actionConfig)[0x3D], &values[0], &values[1],
                              &values[2], &values[3]);
    p->derivedStats[0] = values[1];
    p->derivedStats[1] = values[2];
    p->derivedStats[2] = values[0];
}
