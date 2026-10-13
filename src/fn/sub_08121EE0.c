#include "global.h"
void AnimData_Load(void *, u32);
u32 ScriptProc_Start(u32, u32);
void sub_08121EE0(u8 *p)
{
    AnimData_Load(p + 0x17C, 0x863A);
    *(u32 *)(p + 0x18) = ScriptProc_Start(0x922E, 0x931E);
}
