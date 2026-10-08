#include "global.h"
u8 *sub_08219FBC(u32, u32);
void sub_0821A04C(u8 *, void (*)(void), void (*)(void));
s32 sub_08056E0C(u8 *);
void sub_0821A0C0(u8 *);
void sub_08056DBC(void);
void sub_08056DFC(void);
u8 *sub_08056EE4(void)
{
    u8 *p = sub_08219FBC(9, 0xb0);
    if (p != 0) {
        sub_0821A04C(p, sub_08056DBC, sub_08056DFC);
        if (sub_08056E0C(p) != 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
