#include "global.h"
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_08112758(u8 *);
void sub_0821A0C0(u8 *);
void sub_08112714(void);
void sub_08112734(void);
u8 *sub_081128B8(void)
{
    u8 *p = sub_08219FBC(0xb, 0x1214);
    if (p != 0) {
        sub_0821A04C(p, sub_08112714, sub_08112734);
        if (sub_08112758(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
