#include "global.h"

u8 *sub_08219C40(u32);
void sub_08219DD8(u8 *, u32);
s32 sub_080B43BC(u8 *);
void sub_080B43B0(u8 *);
void sub_08219D38(u8 *);

void sub_080B54A8(void)
{
    u8 *p = sub_08219C40(0x720);
    if (p != 0) {
        sub_08219DD8(p, 0x720);
        if (sub_080B43BC(p) < 0) {
            sub_080B43B0(p);
            sub_08219D38(p);
        }
    }
}
