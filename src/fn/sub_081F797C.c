#include "global.h"
extern const u32 gUnk_08613EA8[];
struct S {
    u8 pad[0xAE];
    u8 b;
};
void Callback_InvokeActor(void *, u32);
void Ratatosk_UpdateScriptedState(struct S *p)
{
    Callback_InvokeActor(p, gUnk_08613EA8[p->b]);
}
