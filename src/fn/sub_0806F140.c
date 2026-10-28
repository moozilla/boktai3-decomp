#include "player.h"
extern u32 gUnk_02000560[4];
struct Player *SceneActorQuery_FindPlayerInBounds(const struct SceneVec *center,
                                                  const struct SceneVec *radius, s32 slot)
{
    struct Player *p = gPlayers[slot];
    if (!p)
        goto absent;
    {
        u32 mask = 8;
        u32 *flags = &gUnk_02000560[slot];
        if (*flags & mask)
            goto absent;
    }
    {
        u32 mask = 0x4000;
        if (!(p->statusFlags & mask))
            goto absent;
    }
    if (center->x - radius->x > p->position.x || center->x + radius->x < p->position.x)
        goto absent;
    if (center->z - radius->z > p->position.z || center->z + radius->z < p->position.z)
        goto absent;
    return p;
absent:
    return 0;
}
