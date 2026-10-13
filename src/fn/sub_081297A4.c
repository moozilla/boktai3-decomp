#include "global.h"

struct S {
    u8 filler[0x18];
    u8 a[0x1c];
    u32 b;
    u32 c;
};
extern struct S *gUnk_02000174;
void AnimData_Load(u8 *, u32);
u32 ScriptProc_Start(u32, u32);

s32 sub_081297A4(struct S *s)
{
    AnimData_Load(s->a, 0x000084B4);
    s->b = ScriptProc_Start(0x0000922E, 0x00004EC0);
    s->c = 0;
    gUnk_02000174 = s;
    return 0;
}
