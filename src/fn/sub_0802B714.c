#include "global.h"
extern const u32 gUnk_08605024[];
void Callback_InvokeActor(void *, u32);
void GridAttack_DispatchReactionAction(u8 *p)
{
    Callback_InvokeActor(p, gUnk_08605024[p[0xae]]);
}
