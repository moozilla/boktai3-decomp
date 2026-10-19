#include "global.h"

typedef struct { u32 w[8]; } Blk;
typedef struct {
    u32 w0;
    u16 h4;
    u16 h6;
    u32 w8;
    u8 pad[0xe8 - 12];
} Ent;
typedef struct {
    u8 pad0[0x18];
    Blk blk;
    u8 pad1[0x5c - 0x38];
    u16 a, b, c, d;
    Ent e[16];
} Obj;
typedef struct {
    u32 w0;
    u16 h4;
    u16 h6;
    u16 h8;
    u16 ha;
    u8 pad[0xe8 - 12];
} Ent2;

extern Blk *sub_0821A520(int, int);
extern void sub_082196C4(void *, void *);
extern void sub_0821983C(void *, void *, int, int, int, int, int, void *);
extern void sub_08220D78(void *, void *, int, int, int);

int sub_081A4DF0(Obj *p, u32 a, u32 b, u32 c)
{
    Blk *src;
    u32 buf[2];
    u32 zero;
    int i;
    Ent *q;
    Ent2 *r;
    Blk *blk;
    u16 *dp;
    src = sub_0821A520(0xCB05, 0xF771);
    if (src == 0)
        return -1;
    p->blk = *src;
    sub_082196C4(&p->blk, src);
    p->a = a;
    p->b = b;
    p->c = c;
    zero = 0;
    CpuSet(&zero, buf, 0x05000002);
    dp = &p->d;
    q = p->e;
    r = (Ent2 *)((u8 *)p + 0x140);
    i = 15;
    do {
        sub_0821983C(q, &p->blk, 0, 0, 2, 0, 0x3c, buf);
        sub_08220D78(q, &p->blk, 0x21, 2, 0);
        q->w8 |= 1;
        r->h8 = 0;
        r->ha = 0;
        r->w0 = 0;
        r++;
        q++;
    } while (--i >= 0);
    *dp = 0;
}
