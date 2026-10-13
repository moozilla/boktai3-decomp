#include "global.h"
struct A {
    u8 p[0x10];
    void *f10;
};
extern struct A *gUnk_030052F4;
void *ScriptProc_Start(u32, u16);
u32 sub_0821E594(u16 v)
{
    gUnk_030052F4->f10 = ScriptProc_Start(0xD4FB, v);
    return 0;
}
