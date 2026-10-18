#include "global.h"
extern const u32 gUnk_08604EF8[];
void sub_0824923C(void *, u32);
void SceneActorState_UpdatePassiveActions(u8 *p)
{
    sub_0824923C(p, gUnk_08604EF8[p[0xAD]]);
}
