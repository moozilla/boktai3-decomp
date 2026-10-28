#include "global.h"
extern u8 *gPlayers[];
s32 SceneActorTarget_FindNearestEligiblePlayer(u8 *);
void Callback_InvokeActor(u8 *, u32);
s32 SceneActorTarget_TrySelectNearestParticipant(u8 *p)
{
    s32 r = SceneActorTarget_FindNearestEligiblePlayer(p + 0x5c);
    s32 ret;
    if (r >= 0) {
        *(u32 *)(p + 0xdc) = (u32)(gPlayers[r] + 0x24);
        ret = 1;
    } else {
        Callback_InvokeActor(p, *(u32 *)(p + 0x5f8));
        ret = 0;
    }
    return ret;
}
