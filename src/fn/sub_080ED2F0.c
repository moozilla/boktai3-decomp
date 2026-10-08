#include "global.h"

u8 *sub_08219C40(u32);
void sub_08219DD8(u8 *, u32);
s32 sub_080EC3F8(u8 *);
void sub_080EC364(u8 *);
void sub_08219D38(u8 *);

void sub_080ED2F0(void)
{
    u8 *p = sub_08219C40(0x63c);
    if (p != 0) {
        sub_08219DD8(p, 0x63c);
        if (sub_080EC3F8(p) < 0) {
            sub_080EC364(p);
            sub_08219D38(p);
        }
    }
}
