#include "global.h"

struct S {
    u8 filler[0x18];
    u8 a[0x1c];
    u32 b;
    u32 c;
};
extern struct S *gUnk_0200016C;
void AnimData_Load(u8 *, u32);
u32 ScriptProc_Start(u32, u32);

s32 sub_08128124(struct S *s)
{
    AnimData_Load(s->a, 0x0000D45E);
    s->b = ScriptProc_Start(0x0000922E, 0x00001DC3);
    s->c = 0;
    gUnk_0200016C = s;
    return 0;
}
