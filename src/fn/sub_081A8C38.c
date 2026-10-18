#include "global.h"
extern const u32 gUnk_08612004[];
s32 sub_081A7EA0(u8 *);
void Callback_InvokeActor(u8 *, u32);
void Vanargand_RunInterruptibleModeCallback(u8 *p)
{
    if (sub_081A7EA0(p) == 0)
        Callback_InvokeActor(p, gUnk_08612004[*(u8 *)(p + 0xad)]);
}
