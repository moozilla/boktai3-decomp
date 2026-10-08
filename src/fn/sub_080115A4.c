#include "global.h"

struct S { u8 out[0x18]; u8 in[0x30]; u8 g[0x7c-0x48]; u32 p; };
u32 sub_08011578(u32, u32, u32);

s32 sub_080115A4(struct S *s)
{
    u8 *in = s->in;
    s32 k = 2;
    s32 i;
    for (i = 0; i <= 0x17; i++, in += k) {
        u32 a = in[0];
        u32 b = in[1];
        u32 r = sub_08011578(s->p, a, b);
        if (r > 0x3f)
            return -1;
        s->out[i] = r;
    }
    return 0;
}
