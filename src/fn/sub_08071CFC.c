#include "global.h"

struct P { u8 f[0x2a]; u16 n; u32 fl; };
void sub_08071470(struct P *);
void sub_0806FD5C(struct P *);
void sub_0806FDE0(struct P *);
void sub_0806FE88(struct P *);
void sub_08071CB0(struct P *);
void sub_08070FA8(struct P *);
s32 sub_08071CFC(struct P *p)
{
    u32 m;
    sub_08071470(p);
    sub_0806FD5C(p);
    sub_0806FDE0(p);
    sub_0806FE88(p);
    sub_08071CB0(p);
    sub_08070FA8(p);
    m = 0xFFFF3FFF;
    p->fl = p->fl & m;
    p->n++;
    return 0;
}