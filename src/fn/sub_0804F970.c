#include "global.h"
struct S { u8 f[0x868]; u32 fl; };
extern struct S *gUnk_02000710;
void sub_0804F7D0(u8 *, u32, void (*)(void));
void sub_0804FB0C(void);
static inline u8 tst(u32 *a, u32 m) { if (*a & m) return TRUE; return FALSE; }
void sub_0804F970(u8 *p)
{
    u32 m = 0x1000;
    if (tst(&gUnk_02000710->fl, m)) {
        u16 *c = (u16 *)(p + 0xf4);
        u32 v = *c + 1;
        *c = v;
        if ((s32)(v << 16) > 0x012B0000)
            sub_0804F7D0(p, 0, sub_0804FB0C);
    }
}
