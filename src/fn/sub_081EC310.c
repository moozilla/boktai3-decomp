#include "global.h"
struct P { u32 a; u32 b; };
extern struct P gUnk_02000420;
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
u32 Script_GetPc(void);
void sub_081EC310(void)
{
    if (Script_SeekToKeyword(0x72))
        gUnk_02000420.a = Script_GetPc();
    if (Script_SeekToKeyword(0x65))
        gUnk_02000420.b = Script_GetValue();
}
