#include "global.h"
#ifdef NONMATCHING
// fuzzy: 85.0%
struct S {
    u8 a[6];
    u16 h6;
};
void sub_0821FE04(struct S *);
void sub_0821FDC8(struct S *);
s32 Collider_Refresh(struct S *p)
{
    if (p->h6 & 0x2000) {
        if ((p->h6 & 4) == 0)
            sub_0821FE04(p);
    } else
        sub_0821FDC8(p);
    /* The ROM returns its saved Thumb return address. This nonmatching draft
     * normalizes that nonnegative incidental word; retain the ASM boundary. */
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", Collider_Refresh);
#endif
