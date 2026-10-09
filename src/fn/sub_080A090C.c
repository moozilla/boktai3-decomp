#include "global.h"
extern u8 *gUnk_02000580;
void sub_0807FC90(void);
struct S {
 u8 pad[0x9C]; s16 z; u8 pad2[0xDC-0x9E]; u8 extra[0x10]; u8 pad3[0x104-0xEC]; u32 at104; u8 pad4[0x114-0x108]; u32 flags114;
 u8 pad5[0x21C-0x118]; void (*callback)(void); u8 pad6[0x2A8-0x220]; u8 a,b,c,pad7; u32 count; u8 done, queued, taken; u8 pad8[2]; u8 code;
 u8 pad9[0x2C8-0x2B6]; u16 timer; u8 pad10[0x2D0-0x2CA]; u32 flags; u16 hflags; u8 pad11[0x3D0-0x2D6]; struct S *other; u32 at3D4;
};
static inline u8 TestExtra(struct S *s, u8 **out) {
 u8 *p = s->extra;
 u32 mask = 8;
 u32 value = p[15] & mask;
 *out = p;
 if (value) return 1;
 return 0;
}
void sub_080A090C(struct S *s) {
 u32 mask = 0x100;
 u8 *extra;
 u32 zero;
 if (s->hflags & mask) return;
 zero = TestExtra(s, &extra);
 if (zero == 0) {
  void (*fn)(void) = sub_0807FC90;
  u8 code = 0x10;
  u8 one;
  u8 zero8;
  u32 *flags;
  u8 *taken;
  u32 three;
  taken = &s->taken;
  one = 1;
  *taken = one;
  s->done = zero;
  s->code = code;
  s->callback = fn;
  zero8 = 0;
  s->timer = zero;
  flags = &s->flags114;
  { u32 clear = -2; *flags &= clear; }
  s->at104 = zero;
  three = 3;
  s->a = zero8;
  s->b = zero8;
  s->c = three;
  s->count = zero;
  s->queued = one;
  { u32 bit = 8; bit |= extra[15]; extra[15] = bit; }
  { u32 bit = 0x100000; s->flags |= bit; }
 }
}
