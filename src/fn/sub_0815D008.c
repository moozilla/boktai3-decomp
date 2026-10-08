#include "global.h"
s32 sub_0815A400(void);
s32 sub_0815CA08(void);
s32 sub_0815C9EC(void);
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void sub_0814F828(s32);
extern s32 gUnk_02000580[];
void sub_0815B5F8(s32, s32, s32, s32, s32);
void sub_0815D008(void)
{
    s32 i = sub_0815A400();
    s32 p = gUnk_02000580[i];
    s32 a, b, c;
    if (p != 0) {
        if (Script_SeekToKeyword(0x66) != 0)
            b = Script_GetValue();
        else
            b = 0x32;
        if (Script_SeekToKeyword(0x4c) != 0)
            c = Script_GetValue();
        else
            c = 0;
        a = sub_0815C9EC();
        sub_0815B5F8(p, a, b, c, sub_0815CA08());
        sub_0814F828(p);
    }
}
