#include "global.h"
// Sabata attack helper. Field meanings remain tentative.
#define Player_GetPosition Player_GetEffectivePosition
struct SceneVec;
struct SceneVec *Player_GetEffectivePosition(s32);
s32 SabataPosition_TestPath(void *, struct SceneVec *);
s32 Sabata_TestPlayerInAttackArea(void *volume)
{
    return SabataPosition_TestPath(volume, Player_GetEffectivePosition(0));
}
