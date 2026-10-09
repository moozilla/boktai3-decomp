#include "global.h"
extern u8 *gUnk_02000710;
u8 sub_0822BDB8(void);
u8 sub_0822BDE8(void);
void sub_0821B148(s32);
s32 sub_081EB864(void)
{
  s32 r;
  s16 flag;
  int failure;
  if (sub_0822BDB8())
  {
    *((u16 *) (gUnk_02000710 + 0x86e)) = 1;
    r = 1;
  }
  else
  {
    u8 v = sub_0822BDE8();
    if (v)
    {
      *((u16 *) (gUnk_02000710 + 0x86e)) = 1;
      r = 0;
    }
    else
    {
      *((u16 *) (gUnk_02000710 + 0x86e)) = v;
      failure = -1;
      r = failure;
    }
  }
  flag = *((s16 *) (gUnk_02000710 + 0x86e));
  if (flag)
  {
    flag = *((s16 *) (gUnk_02000710 + 0x86e));
    sub_0821B148(flag);
  }
  return r;
}
