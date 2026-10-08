#include "global.h"
extern u32 gUnk_020000D8;
void *sub_08219FBC(u32, u32);
void sub_0821A04C(void *, void (*)(void), void (*)(void));
s32 sub_0802C7E8(void *);
void sub_0821A0C0(void *);
void sub_0802C738(void);
void sub_0802C7A4(void);

void *sub_0802C818(void)
{
    void *p;
    if (gUnk_020000D8 != 0)
        return (void *)gUnk_020000D8;
    p = sub_08219FBC(8, 0x38);
    if (p != 0) {
        sub_0821A04C(p, sub_0802C738, sub_0802C7A4);
        if (sub_0802C7E8(p) < 0) {
            sub_0821A0C0(p);
            return 0;
        }
    }
    return p;
}
