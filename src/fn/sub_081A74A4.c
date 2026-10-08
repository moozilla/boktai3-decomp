#include "global.h"
struct S { u8 f0[0xAA]; u8 st; u8 g[4]; u8 af; u8 b0; u8 f2[0x118 - 0xB1]; u8 d[1]; };
void sub_081A68CC(void *);
void sub_08020D68(void *, s32);
static inline u8 chk(struct S *p)
{
    u8 *q = &p->b0;
    if (*q != 0) {
        *q = 0;
        *(q - 1) = 0;
        return TRUE;
    }
    return FALSE;
}
void sub_081A74A4(struct S *p)
{
    u8 *q;
    if (chk(p) == 1) {
        u32 v = 0x1b;
        sub_081A68CC((u8 *)p + 0x21C);
        p->st = v;
    }
    if (p->af != 0)
        sub_08020D68((u8 *)p + 0x118, 1);
}
