#include "global.h"
void sub_0821211C(u8 *arg)
{
  u8 *p = arg;
  s32 v = *((s16 *) (p + 0x384));
  u32 out = 0x1f;
 do { if (v > 9) { out = 0x1b; if (v > 0x11) { out = 0x12; if (v > 0x19) { out = 10; if (v > 0x21) { out = 0x1b; if (v <= 0x29) { out = 0x12; } } } } } (*((u16 *) (p + 0x384)))++; if ((*((s16 *) (p + 0x384))) > 0x31) { *((u16 *) (p + 0x384)) = 0; } } while (0);
  *((u16 *) (p + 0x37c)) = out;
}
