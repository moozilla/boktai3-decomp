#include "global.h"
s32 sub_0815A400(void);
s32 sub_0815CA08(void);
s32 sub_0815C9EC(void);
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void sub_0814F828(s32);
extern s32 gUnk_02000580[];
s32 sub_0815C428(s32, s32);
void sub_0814F838(s32);
void sub_0815D358(void)
{
    s32 i = sub_0815A400();
    s32 p = gUnk_02000580[i];
    s32 r;
    if (p != 0) {
        r = sub_0815C428(p, sub_0815CA08());
        sub_0814F828(p);
        if (r == 0)
            sub_0814F838(p);
    }
}
