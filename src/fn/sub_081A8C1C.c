#include "global.h"
extern const u32 gUnk_08611FE8[];
void Callback_InvokeActor(void *, u32);
void Vanargand_RunSecondaryPhaseCallback(u8 *p)
{
    Callback_InvokeActor(p, gUnk_08611FE8[p[0xae]]);
}
