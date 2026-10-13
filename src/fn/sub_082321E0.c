#include "global.h"
extern void *gUnk_0200005C;
u32 AnimData_FindEntry(u32);
u32 ScriptProc_Start(u32, u32);
u32 sub_082321E0(u32 *s)
{
    gUnk_0200005C = s;
    s[8] = 0;
    s[6] = AnimData_FindEntry(0x1c1a);
    s[7] = ScriptProc_Start(0x922e, 0xd1b8);
    return 0;
}
