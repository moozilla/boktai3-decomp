#include "global.h"
struct V { u32 a, b; };
struct O { void (*f0)(void); void (*f1)(void); u8 g[0x38]; struct V v; };
struct O *sub_0818F218(void *);
void sub_082151E4(void *, s32);
void sub_08215284(void *, s32);
void sub_082144A4(void *, void *, s32);
void sub_08220D20(void *, s32, s32, s32, s32);
void sub_0818EE38(void);
void sub_0818EF58(void);
void sub_0818F2C0(u8 *p, struct V *q)
{
    struct O *o = sub_0818F218(p);
    if (o != 0) {
        u8 *r = (u8 *)o + 8;
        void (*a)(void);
        void (*b)(void);
        sub_082151E4(r, 0xC7ED);
        sub_08215284(r, 0x1A8);
        sub_082144A4((u8 *)o + 0x24, r, 0);
        sub_08220D20((u8 *)o + 0x50, *(s32 *)(p + 0x53AC), 1, 0, 0);
        o->v = *q;
        a = sub_0818EE38;
        b = sub_0818EF58;
        o->f0 = a;
        o->f1 = b;
    }
}
