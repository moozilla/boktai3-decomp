#include "global.h"
// Tentative SceneVolume family: ownership and field semantics remain provisional.
#define SceneVolume_TestPlayerPosition sub_0817A900
#define SceneVolume_TestPosition sub_0817A8C8
#define Player_GetPosition Player_GetEffectivePosition
struct SceneVec;
struct SceneVec *Player_GetEffectivePosition(s32);
s32 sub_0817A8C8(void *, struct SceneVec *);
s32 sub_0817A900(void *volume)
{
    return sub_0817A8C8(volume, Player_GetEffectivePosition(0));
}
