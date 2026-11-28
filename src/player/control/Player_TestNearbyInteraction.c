#include "global.h"
#include "player/player.h"

s32 TetherActor_TestNearbyInteraction(struct SceneVec *, s32, s32);

s32 Player_TestNearbyInteraction(struct Player *p)
{
    return TetherActor_TestNearbyInteraction(&p->position, p->tetherStatusTag,
                                             p->tetherStatusFlags);
}
