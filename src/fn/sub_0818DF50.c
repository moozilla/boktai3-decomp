#include "global.h"
struct P { u8 f[0xc]; void *c; u8 g[0x128-0x10]; s16 w128; u8 h[0x15c-0x12a]; u8 *q; u8 i[0x1cc-0x160]; u32 z; };
void sub_08215284(void *, u32);
void sub_0818DF50(struct P *p)
{
    s32 v = --p->w128;
    if (v == 0) {
        u8 *q = p->q;
        sub_08215284(p->c, *(u16 *)(q + 0x5BC));
        p->z = v;
    }
}
