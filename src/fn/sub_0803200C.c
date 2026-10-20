#include "global.h"
#ifdef NONMATCHING
// fuzzy: 93.5%
// draft: operand order of the id add and the loop reload of *v differ
s32 Script_ParseStringRef(const void *);
u8 *Text_LookupString(s32);
#define Text_CopyStringPair sub_0803200C
// Copies two looked-up strings (ids offset by c and d) back to back into dst.
s32 Text_CopyStringPair(u8 *dst, const void *a, const void *b, s32 c, s32 d)
{
    s32 v;
    v = Script_ParseStringRef(a);
    c = v + c;
    v = (s32)Text_LookupString(c);
    if (v == 0)
        goto fail;
    while (*(u8 *)v != 0) {
        *dst = *(u8 *)v;
        v++;
        dst++;
    }
    v = Script_ParseStringRef(b);
    v = (s32)Text_LookupString(v + d);
    if (v == 0) {
    fail:
        *dst = v;
        return -1;
    }
    while (*(u8 *)v != 0) {
        *dst = *(u8 *)v;
        v++;
        dst++;
    }
    *dst = *(u8 *)v;
    return 0;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0803200C);
#endif
