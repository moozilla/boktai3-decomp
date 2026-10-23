#include "global.h"
#ifdef NONMATCHING
// fuzzy: 87.3%
// fix.py best version (reprinted C)
// 14 differing instruction runs; first diff at +0x10 (mov r5, sp vs ldr r3, [r1, #8])
struct Slot {
    u8 state, b1;
    u16 h2;
    s16 h4;
    u8 p6[2];
    u16 x, y, z, hE;
    u32 f10;
    u8 p14[0x28 - 0x14];
    u16 ox, oy, oz;
    u8 p2E[0x48 - 0x2E];
};
struct S {
    u8 p0[8];
    s16 *vec;
    struct Slot slots[4];
};
struct V {
    u16 x, y, z, pad;
};
extern u16 gUnk_03005418[];
typedef void (*Fn)(void *, struct S *, struct Slot *);
extern Fn const gUnk_08E879A8[];
static inline s32 scale(s32 x)
{
    s32 r;
    if (x >= 0)
        r = x >> 5;
    else
        r = -((-x) >> 5);
    x *= 3;
    return r;
}

void sub_08231EF8(void *a, struct S *s)
{
    struct V v;
    u16 *q;
    s16 *p = s->vec;
    s32 x = p[0];
    s32 z = p[2];
    s32 y;
    struct Slot *slot;
    struct Slot *callslot;
    int i;
    typeof(slot->x + q[0]) t1;
    typeof(slot->z + q[2]) t2;
    {
        u16 *q = (u16 *)(&v);
        q[0] = scale(x - z);
        z = scale(x + z);
        y = scale(p[1]);
        q[1] = z - y;
        z += y;
        q[0] = (q[0] - gUnk_03005418[0]) + 0x78;
        q[1] = (q[1] - gUnk_03005418[1]) + 0x50;
        z -= gUnk_03005418[2];
        q[2] = z;
    }
    {
        const Fn *table = gUnk_08E879A8;
        q = (u16 *)(&v);
        slot = s->slots;
        callslot = slot;
        i = 3;
        do {
            table[slot->state](a, s, callslot);
            if (slot->state) {
                t1 = slot->x + q[0];
                slot->ox = t1;
                slot->oy = q[1] + slot->y;
                t2 = slot->z + q[2];
                slot->oz = t2;
                slot->h2++;
            }
            slot++;
            callslot++;
            i--;
        } while (i >= 0);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_08231EF8);
#endif
