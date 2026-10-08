#include "global.h"
struct P { u8 f[0x22e]; u16 f22e; u8 f230[0x1428 - 0x230]; u32 f1428; };
void sub_08058130(struct P *, u32, u32);
void sub_08058150(struct P *, void (*)(void));
void sub_080582EC(void);
void sub_080582B8(struct P *p)
{
    u32 z;
    u32 *q;
    sub_08058130(p, 2, 6);
    sub_08058150(p, sub_080582EC);
    q = &p->f1428;
    z = 0;
    *q = z;
    p->f22e = z;
}
