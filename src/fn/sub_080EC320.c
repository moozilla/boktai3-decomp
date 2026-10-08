#include "global.h"

void sub_0824923C(void *, u32);
void sub_0807FC5C(void *);
void sub_080EB13C(void *);
void sub_080EB150(void *);

struct S {
    u8 filler[0x27c];
    u32 *tbl;
    u8 filler2[0x2a8 - 0x280];
    u8 idx;
};

s32 sub_080EC320(struct S *s)
{
    sub_080EB13C(s);
    sub_0824923C(s, s->tbl[s->idx]);
    sub_0807FC5C(s);
    sub_080EB150(s);
    return 1;
}
