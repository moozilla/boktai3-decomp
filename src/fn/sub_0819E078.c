#include "global.h"

void sub_08020D68(void *, u32);

struct S { u8 pad[0xAA]; u8 st; u8 pad2[4]; u8 f1; u8 f2; u8 pad3[0xC4 - 0xB1]; u32 cnt; };

static inline u8 TakeFlag(struct S *p)
{
    if (p->f2)
    {
        p->f2 = 0;
        p->f1 = 0;
        return TRUE;
    }
    return FALSE;
}

void sub_0819E078(struct S *p)
{
    if (TakeFlag(p))
    {
        u32 v = 0x1c;
        p->st = v;
        sub_08020D68((u8 *)p + 0x118, 1);
    }
    p->cnt++;
}
