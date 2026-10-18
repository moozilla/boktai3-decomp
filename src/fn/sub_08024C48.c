#include "global.h"
extern u32 gUnk_02000560;
extern const u32 gUnk_08604EF0[];
s32 SceneActorState_TryStartDeathOrFlaggedAction(void *);
void sub_0824923C(void *, u32);
void SceneActorState_UpdateActiveActions(u8 *p)
{
    u32 m = 4;
    if (!(gUnk_02000560 & m)) {
        if (SceneActorState_TryStartDeathOrFlaggedAction(p) == 0) {
            sub_0824923C(p, gUnk_08604EF0[p[0xad]]);
        }
    }
}
