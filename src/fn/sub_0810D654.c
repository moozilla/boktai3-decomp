#include "global.h"

struct P { u8 f[0x42c]; u16 v; };
void sub_0810E1A0(u8 *);
s32 sub_0811D524(void);
void sub_08219728(u8 *, u8 *, u32);
void sub_0810D654(struct P *p)
{
    sub_0810E1A0((u8 *)p);
    p->v = sub_0811D524() + 0x22;
    sub_08219728((u8 *)p + 0x260, (u8 *)p + 0x400, p->v);
}
