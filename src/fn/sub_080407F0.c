#include "global.h"

u8 *ScriptProc_Start(u32, u32);
void sub_082161B4(u32, u32, u8 *, u32, u32, u32, u32 *);

u32 sub_080407F0(u8 *p)
{
    u32 v;
    v = 3;
    *(u8 **)(p + 0x60) = ScriptProc_Start(0xC091, 0xA597);
    sub_082161B4(0, 0, *(u8 **)(p + 0x60), 0, 0, 1, &v);
    return 0;
}
