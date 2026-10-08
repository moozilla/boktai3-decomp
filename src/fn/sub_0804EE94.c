#include "global.h"

struct G { u8 filler[0x868]; u32 f; };
extern struct G *gUnk_02000710;
void sub_0804EDCC(void *);
void sub_0804EDD0(void *);
void sub_0804ECD0(void *);

s32 sub_0804EE94(void *p)
{
    if (gUnk_02000710->f & 2)
        sub_0804EDCC(p);
    else
        sub_0804EDD0(p);
    sub_0804ECD0(p);
    return 0;
}
