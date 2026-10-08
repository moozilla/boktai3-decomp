#include "global.h"
extern u8 *gUnk_02000710;
s32 Script_SeekToKeyword(s32);
u32 Script_GetPc(void);
u32 Script_ParseStringRef(u32);
u32 Text_LookupString(u32);
void sub_08011794(u32, u32, u32, u32);
void sub_08011814(void)
{
    u32 a, b, c;
    s32 i;
    if (!Script_SeekToKeyword(0x63)) return;
    a = Script_GetPc();
    if (!Script_SeekToKeyword(0x6e)) return;
    b = Script_GetPc();
    if (b == 0) return;
    if (!Script_SeekToKeyword(0x64)) return;
    c = Script_GetPc();
    if (c == 0) return;
    i = 0;
    do {
        u32 r = Text_LookupString(Script_ParseStringRef(c) + i);
        sub_08011794(r, a, b, (u32)(gUnk_02000710 + 0x780));
        i++;
    } while (i <= 0x1e49);
}
