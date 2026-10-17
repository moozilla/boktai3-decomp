#include "global.h"
struct S {
    u8 f[0x18];
    u32 a;
    u32 b;
    u32 c;
};
u32 ScriptProc_Start(u32, u32);
void Bg_SetupLayers(u32, u32, u32, u32, u32, u32, void *);
void sub_08111C44(struct S *s)
{
    u32 x;
    s->a = ScriptProc_Start(0xC091, 0xCE15);
    s->b = ScriptProc_Start(0xC091, 0xA597);
    x = 4;
    Bg_SetupLayers(2, 0, s->a, 0, 0, 1, &x);
    s->c = ScriptProc_Start(0x92B3, 0xA2F6) + 0x14;
    CpuSet((void *)s->c, (void *)0x03004FC0, 0x100);
}
