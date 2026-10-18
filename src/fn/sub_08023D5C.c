#include "global.h"
extern const u32 gUnk_08604E44[];
void sub_0824923C(void *, u32);
void SceneActorState_DispatchPhaseHandler(u8 *p)
{
    sub_0824923C(p, gUnk_08604E44[p[0xae]]);
}
