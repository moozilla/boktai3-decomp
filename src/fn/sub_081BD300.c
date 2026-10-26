#include "global.h"
#include "render.h"
u32 ScriptProc_Start(u32, u32);
void DarkLoan_StartDisplayScript(u8 *p)
{
    u16 *r = &gDispCntShadow;
    u32 m = ~0x100;
    *r = m & *r;
    *(u32 *)(p + 0x8e0) = ScriptProc_Start(0xC091, 0x8544);
}
