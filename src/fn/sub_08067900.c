#include "global.h"

struct P {
    u8 f[0x34];
    u32 a;
    u32 b;
};
extern struct P *gUnk_02000128;
void AnimData_Load(u8 *, s32);
u32 ScriptProc_Start(s32, s32);

s32 sub_08067900(struct P *p)
{
    AnimData_Load((u8 *)p + 0x18, 0x1c1b);
    p->b = 0;
    p->a = ScriptProc_Start(0x922e, 0x3dc2);
    gUnk_02000128 = p;
    return 0;
}
