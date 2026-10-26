#include "global.h"
#include "ram.h"
void AnimData_Load(u8 *, u32);
u32 ScriptProc_Start(u32, u32);
s32 SunDiscPool_Init(u8 *p)
{
    AnimData_Load(p + 0x18, 0xA5B3);
    *(u32 *)(p + 0x34) = ScriptProc_Start(0x922E, 0x1752);
    *(u32 *)(p + 0x38) = 0;
    *(u32 *)(p + 0xC0C) = 0;
    gSunDiscPool = p;
    return 0;
}
