#include "global.h"

struct S080FE64C { u8 filler[0xdc]; u8 *p; };
extern u8 *gUnk_02000580;
void sub_0812F270(struct S080FE64C *);

s32 sub_080FE64C(struct S080FE64C *s)
{
    s->p = gUnk_02000580 + 0x24;
    sub_0812F270(s);
    return 0;
}
