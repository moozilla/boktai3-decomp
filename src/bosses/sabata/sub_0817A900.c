#include "global.h"
// Tentative SceneVolume family: ownership and field semantics remain provisional.
#define Player_GetPosition Player_GetEffectivePosition
struct SceneVec;
struct SceneVec *Player_GetEffectivePosition(s32);
s32 ActorPosition_TestPath(void *, struct SceneVec *);
s32 SceneVolume_TestPlayerPosition(void *volume)
{
    return ActorPosition_TestPath(volume, Player_GetEffectivePosition(0));
}
