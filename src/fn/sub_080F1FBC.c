#include "global.h"

void sub_080F1E00(void);

struct S {
    u8 filler[0xcf0];
    void (*fp)(void);
    u8 f2[0xd54 - 0xcf4];
    u8 b;
};

s32 sub_080F1FBC(struct S *s)
{
    s32 r;
    if (s->b != 0)
        r = 1;
    else {
        s->fp = sub_080F1E00;
        r = 0;
    }
    return r;
}
