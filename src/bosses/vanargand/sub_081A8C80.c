#include "global.h"
extern const u32 gUnk_08612010[];
void Callback_InvokeActor(void *, u32);
void Vanargand_RunSharedModeCallback(u8 *p)
{
    Callback_InvokeActor(p, gUnk_08612010[p[0xAD]]);
}
