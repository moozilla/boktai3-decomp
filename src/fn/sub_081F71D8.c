#include "global.h"
struct P { u8 f[0xaa]; u8 baa; u8 f2[0x4]; u8 baf; u8 bb0; u8 f3[0x13]; u32 c4; u8 f4[0x50]; u8 s118[4]; };
void sub_08020D68(void *, u32);
static inline u8 chk(struct P *p) { if (p->bb0) { p->bb0 = 0; p->baf = 0; return TRUE; } return FALSE; }
void sub_081F71D8(struct P *p)
{
    if (chk(p)) { u32 v = 0x1d; p->baa = v; sub_08020D68(p->s118, 1); }
    p->c4++;
}
