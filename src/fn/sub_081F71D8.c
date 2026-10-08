#include "global.h"
<<<<<<< HEAD
struct P { u8 f[0xaa]; u8 baa; u8 f2[0x4]; u8 baf; u8 bb0; u8 f3[0x13]; u32 c4; u8 f4[0x50]; u8 s118[4]; };
void sub_08020D68(void *, u32);
static inline u8 chk(struct P *p) { if (p->bb0) { p->bb0 = 0; p->baf = 0; return TRUE; } return FALSE; }
void sub_081F71D8(struct P *p)
{
    if (chk(p)) { u32 v = 0x1d; p->baa = v; sub_08020D68(p->s118, 1); }
    p->c4++;
=======

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

void sub_081F71D8(struct S *p)
{
    if (TakeFlag(p))
    {
        u32 v = 0x1D;
        p->st = v;
        sub_08020D68((u8 *)p + 0x118, 1);
    }
    p->cnt++;
>>>>>>> main
}
