#include "global.h"

void sub_080B8D14(void *);
void sub_0824923C(void *, u32);
void sub_0807FC5C(void *);
void sub_080B8DA4(void *);

struct S {
    u8 filler[0x27c];
    u32 *tbl;
    u8 filler2[0x2a8 - 0x280];
    u8 idx;
    u8 filler3[0x2b8 - 0x2a9];
    u16 h2b8;
    u8 filler4[0x2d4 - 0x2ba];
    u16 flags;
};

s32 sub_080C3078(struct S *s)
{
    u32 m;
    sub_080B8D14(s);
    m = 0x2000;
    if ((s->flags & m) || s->h2b8 == 0) {
        sub_0824923C(s, s->tbl[s->idx]);
        sub_0807FC5C(s);
    }
    sub_080B8DA4(s);
    return 1;
}
