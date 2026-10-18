#include "global.h"
extern const u32 gUnk_08612008[];
void Callback_InvokeActor(void *, u32);
void Vanargand_RunSecondaryModeCallback(u8 *p)
{
    Callback_InvokeActor(p, gUnk_08612008[p[0xAD]]);
}
