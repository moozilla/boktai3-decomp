#include "global.h"
extern const u32 gUnk_08613F50[];
struct S {
    u8 pad[0xAD];
    u8 b;
};
void Callback_InvokeActor(void *, u32);
void Ratatosk_UpdateScriptedMode(struct S *p)
{
    Callback_InvokeActor(p, gUnk_08613F50[p->b]);
}
