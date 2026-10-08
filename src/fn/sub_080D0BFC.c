#include "global.h"

u8 *sub_08219C40(u32);
void sub_08219DD8(u8 *, u32);
s32 sub_080CFC14(u8 *);
void sub_080CFC08(u8 *);
void sub_08219D38(u8 *);

void sub_080D0BFC(void)
{
    u8 *p = sub_08219C40(0x6b0);
    if (p != 0) {
        sub_08219DD8(p, 0x6b0);
        if (sub_080CFC14(p) < 0) {
            sub_080CFC08(p);
            sub_08219D38(p);
        }
    }
}
