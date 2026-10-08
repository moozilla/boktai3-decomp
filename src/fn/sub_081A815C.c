#include "global.h"
struct S { u8 f0[0xAA]; u8 st; u8 g[4]; u8 af; u8 b0; u8 f2[0xC4 - 0xB1]; u32 c4; };
void sub_081A68CC(void *);
void sub_081A7C94(void *);
void sub_081A7D80(void *);
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
void sub_081A815C(struct S *p)
{
    struct S *q = p;
    if (chk(p) == 1) {
        u32 v = 5;
        sub_081A68CC((u8 *)p + 0x21C);
        p->st = v;
    }
    if (p->af != 0) {
        sub_081A7C94(q);
        sub_081A7D80(q);
    }
    p->c4++;
}
