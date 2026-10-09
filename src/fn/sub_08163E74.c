#include "global.h"
void sub_081639E4(u32, u32, u32, u32);
void sub_08163E74(u32 a, u32 b, u32 c, u32 d)
{
  s32 i;
  u32 saved;
  saved = a;
  for (i = 2; i >= 0; i--)
  {
    sub_081639E4(saved, b, c, d);
  }

}
