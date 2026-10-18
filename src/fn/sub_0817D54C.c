#include "global.h"
extern u32 gUnk_02000560;
extern const u32 gUnk_08611CB8[];
s32 ActorState_BeginDepletedState(void *);
void Callback_InvokeActor(void *, u32);
void BossSlot2_RunGuardedPhaseCallback(u8 *p)
{
    u32 m = 4;
    if (!(gUnk_02000560 & m)) {
        if (ActorState_BeginDepletedState(p) == 0) {
            Callback_InvokeActor(p, gUnk_08611CB8[p[0xad]]);
        }
    }
}
