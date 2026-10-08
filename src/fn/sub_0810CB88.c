#include "global.h"

extern void *gUnk_020001C4;
void sub_0810C14C(void);
void sub_08033468(void);
void sub_080335B4(void);
void sub_082156B8(s32);
void sub_0821A0C0(void *);

void sub_0810CB88(void)
{
    if (gUnk_020001C4 != 0) {
        sub_0810C14C();
        sub_08033468();
        sub_080335B4();
        sub_082156B8(0);
        sub_082156B8(1);
        sub_082156B8(2);
        sub_082156B8(3);
        sub_0821A0C0(gUnk_020001C4);
    }
}
