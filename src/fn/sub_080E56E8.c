#include "global.h"

void sub_080E41BC(void *);
void sub_0824923C(void *, u32);
void sub_0807FC5C(void *);
void sub_080E41D4(void *);

struct S {
    u8 filler[0x27c];
    u32 *tbl;
    u8 filler2[0x2a8 - 0x280];
    u8 idx;
    u8 filler3[0x2b8 - 0x2a9];
    u16 h2b8;
};

s32 sub_080E56E8(struct S *s)
{
    struct S *s2 = s;
    sub_080E41BC(s);
    if (s->h2b8 == 0) {
        sub_0824923C(s, s->tbl[s->idx]);
        sub_0807FC5C(s);
    }
    sub_080E41D4(s2);
    return 1;
}
