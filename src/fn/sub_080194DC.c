#include "global.h"

u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 ClockEvents_Initialize(u8 *);
void sub_0821A0C0(u8 *);
void ClockEvents_Update(void);
void sub_08019470(void);

u8 *sub_080194DC(void)
{
    u8 *p;
    p = sub_08219FBC(8, 0x30);
    if (p != 0) {
        sub_0821A04C(p, ClockEvents_Update, sub_08019470);
        if (ClockEvents_Initialize(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
