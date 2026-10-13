#include "global.h"
void *ScriptProc_Start(u32, u32);
void sub_082161B4(u32, u32, void *, u32, u32, u32, u32 *);
extern u8 gUnk_03004FC0[];
void sub_0810CBC8(u8 *s)
{
    u32 value;
    void *p;
    void *q;
    p = ScriptProc_Start(0xC091, 0xCE15);
    *(void **)(s + 0x18) = p;
    value = 2;
    sub_082161B4(2, 0, p, 0, 0, 1, &value);
    q = (u8 *)ScriptProc_Start(0x92B3, 0xA2F6) + 0x14;
    *(void **)(s + 0x1C) = q;
    CpuSet(q, gUnk_03004FC0, 0x100);
}
