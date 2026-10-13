#include "global.h"

struct S {
    u8 f[0x18];
    s32 a;
    s32 b;
};

s32 AnimData_FindEntry(s32);
s32 ScriptProc_Start(s32, s32);

void sub_081230FC(struct S *p)
{
    p->b = AnimData_FindEntry(0x1C1A);
    p->a = ScriptProc_Start(0x922E, 0xD1B8);
}
