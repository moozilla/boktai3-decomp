#include "global.h"
struct P { u32 a; u32 b; };
extern struct P gUnk_020003C0;
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
u32 Script_GetPc(void);
void sub_081E269C(void)
{
    if (Script_SeekToKeyword(0x72))
        gUnk_020003C0.a = Script_GetPc();
    if (Script_SeekToKeyword(0x63))
        gUnk_020003C0.b = Script_GetValue();
}
