#include "password.h"
void sub_08011814(void)
{
    u8 *a, *b, *c;
    s32 i;
    if (!Script_SeekToKeyword(0x63))
        return;
    a = Script_GetPc();
    if (!Script_SeekToKeyword(0x6e))
        return;
    b = Script_GetPc();
    if (b == 0)
        return;
    if (!Script_SeekToKeyword(0x64))
        return;
    c = Script_GetPc();
    if (c == 0)
        return;
    i = 0;
    do {
        u8 *r = Text_LookupString(Script_ParseStringRef(c) + i);
        sub_08011794(r, (s32)a, (s32)b, gUnk_02000710 + 0x780);
        i++;
    } while (i <= 0x1e49);
}
