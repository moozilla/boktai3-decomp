#include "global.h"
s32 sub_0815A400(void);
s32 sub_0815CA08(void);
s32 sub_0815C9EC(void);
void sub_0815BBE4(s32, s32, s32);
void sub_0814F828(s32);
extern s32 gUnk_02000580[];
void sub_0815D26C(void)
{
    s32 i = sub_0815A400();
    s32 p = gUnk_02000580[i];
    s32 a;
    if (p != 0) {
        a = sub_0815C9EC();
        sub_0815BBE4(p, a, sub_0815CA08());
        sub_0814F828(p);
    }
}
