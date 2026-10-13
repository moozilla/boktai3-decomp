#include "global.h"
void AnimData_Load(u8 *);
u32 ScriptProc_Start(u32, u16);
u32 sub_0806B5B0(u8 **p, u32 b, u32 c)
{
    AnimData_Load(*p + 0x2c);
    ScriptProc_Start(0x922E, c);
}
