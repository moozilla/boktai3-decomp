#include "global.h"
extern u8 *gUnk_02000580[];
s32 sub_0815A400(void);
void sub_0813D8F4(u8 *, s32);
void sub_0815AA0C(void)
{
    u8 *p;
    s32 i = sub_0815A400();
    p = gUnk_02000580[i];
    if (p) {
        s32 j = 0;
        do {
            sub_0813D8F4(p, j);
            j++;
        } while (j <= 1);
    }
}
