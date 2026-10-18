#include "global.h"
extern const u32 gUnk_08605038[];
void Callback_InvokeActor(void *, u32);
void GridAttack_UpdateReactionActions(u8 *p)
{
    Callback_InvokeActor(p, gUnk_08605038[p[0xAD]]);
}
