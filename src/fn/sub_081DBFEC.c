#include "global.h"
void *sub_08219FBC(s32, s32);
void sub_0821A04C(void *, void *, void *);
void sub_0821A0C0(void *);
void sub_081DBB4C(void);
void sub_081DBDC8(void);
s32 sub_081DBE04(void *);

void *sub_081DBFEC(void)
{
    void *r = sub_08219FBC(0xb, 0x1dc);
    if (r != 0) {
        sub_0821A04C(r, sub_081DBB4C, sub_081DBDC8);
        if (sub_081DBE04(r) < 0) {
            sub_0821A0C0(r);
            return 0;
        }
    }
    return r;
}
