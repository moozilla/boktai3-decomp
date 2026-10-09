#include "global.h"
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void *, void *);
s32 sub_081FEFFC(void *);
void sub_0821A0C0(void *);
void sub_081FEE0C(void);
void sub_081FEFC4(void);

void *sub_081FF02C(void) {
    void *p = sub_08219FBC(8, 0x171c);
    if (p) {
        sub_0821A04C(p, sub_081FEE0C, sub_081FEFC4);
        if (sub_081FEFFC(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
