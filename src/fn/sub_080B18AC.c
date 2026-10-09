#include "global.h"
void sub_0807FC90(void);
struct S {
 u8 pad[0x9C]; s16 z; u8 pad2[0xDC-0x9E]; u8 extra[0x10]; u8 pad3[0x104-0xEC]; u32 at104; u8 pad4[0x114-0x108]; u32 flags114;
 u8 pad5[0x21C-0x118]; void (*callback)(void); u8 pad6[0x2A8-0x220]; u8 a,b,c,pad7; u32 count; u8 done, queued, taken; u8 pad8[2]; u8 code;
 u8 pad9[0x2C8-0x2B6]; u16 timer; u8 pad10[0x2D0-0x2CA]; u32 flags; u16 hflags; u8 pad11[0x3D0-0x2D6]; struct S *other; u32 at3D4;
};
void sub_080B18AC(struct S *s) {
 void (*fn)(void) = sub_0807FC90;
 u8 code = 0x10;
 u32 zero;
 u8 one;
 u8 zero8;
 u32 *flags;
 u8 *taken;
 u32 two;
 taken = &s->taken;
 zero = 0;
 one = 1;
 *taken = one;
 s->done = zero;
 s->code = code;
 s->callback = fn;
 zero8 = 0;
 s->timer = zero;
 flags = &s->flags114;
 { u32 mask = -2; *flags &= mask; }
 s->at104 = zero;
 two = 2;
 s->a = zero8;
 s->b = two;
 s->c = zero8;
 s->count = zero;
 s->queued = one;
}
