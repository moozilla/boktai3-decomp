#include "global.h"
extern u8 *gUnk_02000488;
void sub_08049168(u8 *, u32);
extern u32 gUnk_030053F4;
void sub_080492F0(u8 *);
void sub_0804B58C(u8 *p)
{
    if (!(gUnk_030053F4 & 0x200)) {
        if (*(u16 *)(p + 0x40A) != 0) {
            if (*(u16 *)(p + 0x41C) == 0) {
                if (p[0x3E4] == 0) {
                    u16 *q = (u16 *)(p + 0x40C);
                    if (*q != 0) *q = *q - 1;
                    else sub_080492F0(p);
                }
            }
        }
    }
}
