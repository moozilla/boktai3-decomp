#include "global.h"

extern u8 *gUnk_02000580[];
s32 sub_0815A400(void);
s32 sub_0815C9EC(void);
s32 sub_0815CA08(void);
void sub_0814F828(u8 *);
void sub_0815AF50(u8 *, s32, s32);

void sub_0815CB20(void)
{
    u8 *p;
    s32 i = sub_0815A400();
    s32 a;
    p = gUnk_02000580[i];
    if (p != 0) {
        a = sub_0815C9EC();
        sub_0815AF50(p, a, sub_0815CA08());
        sub_0814F828(p);
    }
}
