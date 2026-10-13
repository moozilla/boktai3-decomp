#include "global.h"
struct S {
    u8 f[0x18];
    u32 a;
    u32 b;
    u32 c;
};
u32 ScriptProc_Start(u32, u32);
void sub_082161B4(u32, u32, u32, u32, u32, u32, void *);
void sub_0810E964(struct S *s)
{
    u32 x;
    u32 r;
    s->a = ScriptProc_Start(0xC091, 0xCE15);
    s->b = ScriptProc_Start(0xC091, 0xA597);
    x = 2;
    sub_082161B4(2, 0, s->a, 0, 0, 1, &x);
    r = ScriptProc_Start(0x92B3, 0xA2F6) + 0x14;
    s->c = r;
    CpuSet((void *)r, (void *)0x03004FC0, 0x100);
}
