#include "global.h"
struct Values { u8 pad[0xa0]; s16 values[1]; };
extern struct Values *gUnk_02000710;
s32 sub_0822CCC8(s32);
void sub_0822CCE0(s32, s32);
void sub_0822D080(s32 a, s32 b)
{
    struct Values *p = gUnk_02000710;
    s16 *x = &p->values[a];
    s32 t = *x;
    s16 *y = &p->values[b];
    *x = *y;
    *y = t;
    t = sub_0822CCC8(a);
    sub_0822CCE0(a, sub_0822CCC8(b));
    sub_0822CCE0(b, t);
}
