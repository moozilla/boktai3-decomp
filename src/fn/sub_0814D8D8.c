#include "global.h"

struct P0814D8D8 { u8 f0[0xAAE]; u16 fAAE; u8 fAB0[0xBF0 - 0xAB0]; u32 fBF0; };
struct B0814D8D8 { u8 f0[0x15]; u8 f15; u8 f16[10]; };
extern u8 gUnk_08E5FA6C[];
void sub_0822D764(u32, void *);

s32 sub_0814D8D8(struct P0814D8D8 *p)
{
    struct B0814D8D8 buf;
    u16 *ip = &p->fAAE;
    if (*ip <= 3) {
        u8 *e;
        sub_0822D764(p->fBF0, &buf);
        e = gUnk_08E5FA6C + (buf.f15 << 6);
        e += *ip * 12 + 0x10;
        if (*e != 0)
            return 1;
    }
    return 0;
}
