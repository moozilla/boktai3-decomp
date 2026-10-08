#include "global.h"
struct G { u32 a, b, c, d, e; };
extern struct G gUnk_02000340;
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void sub_081DF06C(void)
{
    if (Script_SeekToKeyword(0x6f)) gUnk_02000340.c = Script_GetValue();
    if (Script_SeekToKeyword(0x63)) gUnk_02000340.d = Script_GetValue();
    if (Script_SeekToKeyword(0x66)) gUnk_02000340.e = Script_GetValue();
}
