#include "global.h"
extern u8 *gUnk_0200017C;
void AnimData_Load(u8 *, u32);
u32 ScriptProc_Start(u32, u32);
s32 sub_0812A4C0(u8 *p)
{
    AnimData_Load(p + 0x18, 0xA5B3);
    *(u32 *)(p + 0x34) = ScriptProc_Start(0x922E, 0x1752);
    *(u32 *)(p + 0x38) = 0;
    *(u32 *)(p + 0xC0C) = 0;
    gUnk_0200017C = p;
    return 0;
}
