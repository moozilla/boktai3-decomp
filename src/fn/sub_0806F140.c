// MATCHING_CONCESSION: register-pinning
#include "player.h"
extern u32 gUnk_02000560[4];
struct Player *SceneActorQuery_FindPlayerInBounds(const struct SceneVec *center,
                                                  const struct SceneVec *radius, s32 slot)
{
    struct Player *p = gUnk_02000580[slot];
    if (!p)
        goto absent;
    {
        u32 mask = 8;
        u32 *flags = &gUnk_02000560[slot];
        asm("" : "+r"(mask), "+r"(flags));
        if (*flags & mask)
            goto absent;
    }
    {
        u32 mask = 0x4000;
        asm("" : "+r"(mask));
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
