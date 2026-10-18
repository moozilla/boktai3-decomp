#include "global.h"

extern u8 *gUnk_02000580[];
s32 sub_0815A400(void);
s32 sub_0815C9EC(void);
s32 sub_0815CA08(void);
void sub_0814F828(u8 *);
void PlayerCue_Stand(u8 *, s32);

void sub_0815CAF0(void)
{
    u8 *p;
    s32 i = sub_0815A400();
    p = gUnk_02000580[i];
    if (p != 0) {
        PlayerCue_Stand(p, sub_0815C9EC());
        sub_0814F828(p);
    }
}
