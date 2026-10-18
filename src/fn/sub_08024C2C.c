#include "global.h"
extern const u32 gUnk_08604EE0[];
void sub_0824923C(void *, u32);
void SceneActorState_DispatchSecondaryAction(u8 *p)
{
    sub_0824923C(p, gUnk_08604EE0[p[0xae]]);
}
