#include "global.h"
extern const s16 gUnk_086149C4[];
void sub_080AADE4(u8 *s)
{
  u32 *state = (u32 *) (s + 0x8EC);
  u32 stored = *state;
  u32 limit = 0x7FFFFFFF;
  if (stored != limit)
  {
    u8 *p = *((u8 **) (s + 0x8F0));
    s32 off;
    u8 *anglep;
    u16 *radiusp;
    s16 *v;
    u32 angle;
    u32 radius;
    s32 value;
    s32 x;
    u32 zero;
    *state = limit;
    off = 0x108;
    anglep = p + off;
    radiusp = (u16 *) (p + 0xDC);
    off += 0x14;
    v = (s16 *) (p - (-off));
    angle = *anglep;
    radius = *radiusp;
    value = gUnk_086149C4[(angle + 0x40) & 0xFF] * radius;
    if (value >= 0)
    {
      x = value >> 12;
    }
    else
    {
      x = -((-value) >> 12);
    }
    zero = 0;
    v[0] = x;
    v[1] = zero;
    value = gUnk_086149C4[angle] * radius;
    if (value >= 0)
    {
      value >>= 12;
    }
    else
    {
      value = -((-value) >> 12);
    }
    v[2] = value;
  }
}
