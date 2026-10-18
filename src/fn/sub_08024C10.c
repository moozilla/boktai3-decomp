#include "global.h"
extern const u32 gUnk_08604E9C[];
void sub_0824923C(void *, u32);
void SceneActorState_DispatchPrimaryAction(u8 *p)
{
    sub_0824923C(p, gUnk_08604E9C[p[0xae]]);
}
