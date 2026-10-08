#include "global.h"

void sub_080FD10C(void *);
void sub_080FD128(void *);
void sub_0824923C(void *, u32);
void sub_0807FC5C(void *);

s32 sub_080FE614(void *p)
{
    u8 idx;
    u32 *tbl;

    sub_080FD10C(p);
    idx = *(u8 *)((u8 *)p + 0x2A8);
    tbl = *(u32 **)((u8 *)p + 0x27C);
    sub_0824923C(p, tbl[idx]);
    sub_0807FC5C(p);
    sub_080FD128(p);
    return 1;
}
