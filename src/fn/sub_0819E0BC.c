#include "global.h"
struct P { u8 f[0xAA]; u8 a; u8 f1[4]; u8 b; u8 c; };
void sub_08020D68(void *, s32);
static inline u8 chk(struct P *p)
{
    if (p->c != 0) {
        p->c = 0;
        p->b = 0;
        return TRUE;
    }
    return FALSE;
}
void sub_0819E0BC(struct P *p)
{
    if (chk(p)) {
        u8 k = 0x1d;
        p->a = k;
        sub_08020D68((u8 *)p + 0x118, 1);
    }
}
