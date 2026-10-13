#include "global.h"

u8 *ScriptProc_Start(u32, u32);
s32 sub_08058544(void *);

void sub_080585AC(void *p)
{
    u8 *q = ScriptProc_Start(0x92B3, 0x313A) + 0x14;
    q += sub_08058544(p) << 5;
    CpuSet(q, (void *)0x03004FC0, 0x04000010);
}
