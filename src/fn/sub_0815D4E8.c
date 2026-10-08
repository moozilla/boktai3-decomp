#include "global.h"
s32 sub_0815A400(void);
s32 sub_0815CA08(void);
s32 sub_0815C9EC(void);
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void sub_0814F828(s32);
extern s32 gUnk_02000580[];
void sub_0815C348(s32, s32);
void sub_0815D4E8(void)
{
    s32 i = sub_0815A400();
    s32 p = gUnk_02000580[i];
    s32 a;
    if (p != 0) {
        if (Script_SeekToKeyword(0x72) != 0)
            a = Script_GetValue();
        else
            a = 0;
        sub_0815C348(p, a);
        sub_0814F828(p);
    }
}
