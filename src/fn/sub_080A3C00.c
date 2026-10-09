#include "global.h"
inline static u8 blocked2048(u8 *c)
{
  u32 mask;
  u32 *p;
  u32 off;
  if (!c)
    goto zero;
  mask = 0x40;
  off = 0xB5;
  off <<= 2;
  p = (u32 *) (c + off);
  if ((*((u16 *) p)) & mask)
    goto one;
  mask = 0x80;
  mask <<= 10;
  off -= 4;
  p = (u32 *) (c + off);
  if ((*p) & mask)
    goto one;
  if ((*((s16 *) (c + 0x9C))) > 0)
    goto zero;
  one:
  return 1;
  zero:
  return 0;

}

extern u8 *gUnk_02000580;
void sub_080A3C00(u8 *s) {
    if (blocked2048(s) == 0 && *(s8 *)(gUnk_02000580 + 0x4F3) < 0) {
        u8 *q = *(u8 **)(s + 0x3D0);
        u16 *flags = (u16 *)(q + 0x62C);
        u32 mask = -3;
        *flags &= mask;
    }
}
