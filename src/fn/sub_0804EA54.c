#include "global.h"
struct S { u8 f[0x868]; u32 fl; u8 g[0x08]; s16 v; };
struct O { u8 f[0x18]; u32 q; u8 g[0xd]; u8 b29; };
extern struct S *gUnk_02000710;
extern u32 gUnk_02000580;
extern u16 gUnk_02000528;
extern u16 gUnk_02000590;
s32 sub_0804EA38(struct O *);
void sub_0804E8D8(struct O *);
void sub_0804E87C(struct O *);
void sub_0804E730(struct O *);
void sub_0804E800(struct O *);
static inline u32 mk(u32 *a, u32 m) { return *a & m; }
static inline u8 tst(u32 v) { if (v) return TRUE; return FALSE; }
s32 sub_0804EA54(struct O *o)
{
    if (o->b29 != 0) o->b29 = 0;
    o->q = gUnk_02000580;
    if (o->q != 0) {
        struct S *s = gUnk_02000710;
        if (s->v == 0) {
            if (sub_0804EA38(o) != 0) sub_0804E8D8(o);
            else sub_0804E87C(o);
        } else {
            if (s->fl & 2) sub_0804E730(o);
            else sub_0804E800(o);
        }
        {
            u32 v = gUnk_02000528;
            u32 r = 0;
            if (v == 1) r = 1;
            if (r != 0 && gUnk_02000590 != 0) o->b29 = 1;
        }
    }
    return 0;
}
