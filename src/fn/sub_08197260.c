#include "global.h"

extern const u8 gUnk_0824F5C0[];
void Callback_InvokeActor(u8 *, u32);
void sub_08195334(u8 *);

void Nidhoggr_UpdateState(u8 *p)
{
    const u32 *tbl = (const u32 *)gUnk_0824F5C0;
    Callback_InvokeActor(p, tbl[*(p + 0x7249)]);
    sub_08195334(p);
}
