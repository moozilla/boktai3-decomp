#include "global.h"
#include "player/player.h"

struct SceneVec *Player_GetEffectivePosition(u32 i)
{
    struct Player *p = gPlayers[i];
    if (p == 0)
        return 0;
    if (p->tetherMode == 0x26 || p->tetherMode == 0xc)
        return (struct SceneVec *)&p->secondaryAnimation.position;
    return &p->position;
}
