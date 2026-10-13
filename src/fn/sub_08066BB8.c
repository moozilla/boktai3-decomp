#include "global.h"

struct P {
    u8 f[0x34];
    u8 g[0x1c];
    u32 a;
    u32 b;
    u32 c;
};
void AnimData_Load(u8 *, s32);
u32 AnimData_FindEntry(s32);
u32 ScriptProc_Start(s32, s32);

void sub_08066BB8(struct P *p)
{
    AnimData_Load((u8 *)p + 0x18, 0x363c);
    AnimData_Load((u8 *)p + 0x34, 0x363d);
    p->c = AnimData_FindEntry(0x1c1c);
    p->a = ScriptProc_Start(0x922e, 0x6830);
    p->b = ScriptProc_Start(0x922e, 0xd41f);
}
