#include "global.h"

u8 *sub_08219C40(u32);
void sub_08219DD8(u8 *, u32);
s32 sub_080F0C20(u8 *);
void sub_080F0B8C(u8 *);
void sub_08219D38(u8 *);

void sub_080F1BD4(void)
{
    u8 *p = sub_08219C40(0x640);
    if (p != 0) {
        sub_08219DD8(p, 0x640);
        if (sub_080F0C20(p) < 0) {
            sub_080F0B8C(p);
            sub_08219D38(p);
        }
    }
}
