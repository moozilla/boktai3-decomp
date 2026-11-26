#include "global.h"
s32 Script_ParseStringRef(const void *);
u8 *Text_LookupString(s32);
#define Text_CopyStringPair sub_0803200C
// Copies two looked-up strings (ids offset by c and d) back to back into dst.
s32 Text_CopyStringPair(u8 *dst, const void *a, const void *b, s32 c, s32 d)
{
    s32 v;
    u8 ch;
    v = Script_ParseStringRef(a);
    v = (s32)Text_LookupString(v + c);
    if (v == 0)
        goto fail;
    while ((ch = *(u8 *)v) != 0) {
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
    while ((ch = *(u8 *)v) != 0) {
        *dst = *(u8 *)v;
        v++;
        dst++;
    }
    *dst = ch;
    return 0;
}
