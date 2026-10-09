#include "global.h"
struct T { u8 pad[0x8D0]; u16 flags; u8 pad2[4]; u16 timer; };
struct S { u8 pad[0x2A8]; u8 a, b, c, pad2; u32 count; u8 pad3; u8 queued; u8 pad4[0x2D0-0x2B2]; u32 flags; u8 pad5[0x2E8-0x2D4]; u16 mode; u8 pad6[0x3D0-0x2EA]; struct T *other; };
static inline u8 Pred(struct T *p) {
    u32 mask = 2;
    u32 off = 0x8D0;
    if (*(u16 *)((u8 *)p + off) & mask) return 1;
    return 0;
}
void sub_080AABEC(struct S *s) {
    struct T *p = s->other;
    u8 active = Pred(p);
    u16 *flags = &p->flags;
    if (active) {
        u32 four = 4;
        u32 mask;
        u32 *f;
        s->a = 0;
        s->b = 0;
        s->c = four;
        s->count = 0;
        s->queued = 1;
        f = &s->flags;
        mask = 0xFFEFFFFF;
        *f &= mask;
        p->timer = 0xB4;
        s->mode = 2;
        four |= *flags;
        *flags = four;
    }
    { u32 mask = -3; *flags &= mask; }
}
