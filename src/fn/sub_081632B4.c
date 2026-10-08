#include "global.h"
struct P { u8 f0[0x340]; u8 x; };
extern struct P *gUnk_02000210;
void sub_0816310C(struct P *);
void sub_08177DEC(s32);
s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
void sub_082279A8(s32, s32, s32, s32, s32, s32, s32);
void sub_081632B4(void)
{
    struct P *p = gUnk_02000210;
    if (p != 0) {
        sub_0816310C(p);
        sub_08177DEC(7);
        p->x = 1;
        if (Script_SeekToKeyword(0x66) != 0) {
            if (Script_GetValue() != 0)
                sub_082279A8(3, 5, 4, 4, 4, 0x1fff, 2);
        }
    }
}
