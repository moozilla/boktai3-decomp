#include "global.h"
struct P {
    u8 f0[0xad];
    u8 i;
};
extern const u32 gUnk_08611E08[];
void Callback_InvokeActor(void *, u32);
void sub_0819E408(void *);
void Hresvelgr_UpdateSecondaryMode(struct P *p)
{
    Callback_InvokeActor(p, gUnk_08611E08[p->i]);
    sub_0819E408(p);
}
