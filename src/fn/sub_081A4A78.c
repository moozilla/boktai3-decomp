#include "global.h"

struct G { u8 f[0x62]; u16 cnt; };
struct A { u8 f0[8]; u32 flags; u8 f0c[0x64]; u8 f70[0x74]; u16 he4; };
extern struct G *gUnk_02000244;
void sub_08013B74(void *);
void sub_081A492C(void *, s32);

void sub_081A4A78(struct A *p)
{
    gUnk_02000244->cnt--;
    p->he4 = 0;
    p->flags |= 1;
    sub_08013B74(p->f70);
    sub_081A492C(p, 0);
}
