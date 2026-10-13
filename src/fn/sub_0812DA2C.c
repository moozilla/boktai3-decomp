#include "global.h"
u32 ScriptProc_Start(u32, u32);
void Anim_SetSequence(u8 *, u32, u32, u32, u32);
void sub_0812DA2C(u8 *p, u16 b)
{
    u32 r = ScriptProc_Start(0x922E, b);
    *(u32 *)(p + 0x58) = r;
    Anim_SetSequence(p + 0x48, r, 0, 0, 0);
}
