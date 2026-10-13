#include "global.h"
void *ScriptProc_Start(u32, u32);
void sub_082161B4(u32, u32, void *, u32, u32, u32, u32 *);
extern u8 gUnk_03004FC0[];
void sub_081094EC(u8 *s)
{
    u32 value;
    void *p;
    void *q;
    p = ScriptProc_Start(0xC091, 0xA597);
    *(void **)(s + 0x18) = p;
    value = 3;
    sub_082161B4(2, 0, p, 0, 0, 1, &value);
    q = (u8 *)ScriptProc_Start(0x92B3, 0x81B) + 0x14;
    *(void **)(s + 0x1C) = q;
    CpuSet(q, gUnk_03004FC0, 0x100);
}
