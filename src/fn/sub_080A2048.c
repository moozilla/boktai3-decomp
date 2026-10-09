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

void sub_080A2048(u8 *unused, u8 *obj, u8 *ctx)
{
  int mask;
  u32 *flags;
  if ((blocked2048(ctx) == 0) && ((*((u16 *) (obj + 0xA))) & 0xF))
  {
    *((u32 *) (ctx + 0x304)) = *((u16 *) (obj + 4));
    flags = (u32 *) (ctx + 0x2D0);
    mask = 0xFEFFFFFF;
    *flags &= mask;
  }
}
