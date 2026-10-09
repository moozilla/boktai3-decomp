#include "global.h"
struct V
{
  u16 x;
  u16 y;
  u32 z;
};
extern u32 gUnk_03005308;
struct R
{
  u8 a;
  u8 b;
} __attribute__((packed));
extern struct R gUnk_0203B400[];
void sub_081263E0(void *, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32, s32);
void sub_08203660(void *);
s32 sub_08203D6C(u8 *p)
{
 if (!(*((s16 *) (p + 0x144)))) { if ((*((s32 *) (p + 0x18))) <= 11) { struct V v; struct V *q = &v; u32 r;
      *q = *((struct V *) (p + 0x40));
      q->y += 200;
      gUnk_03005308 = (gUnk_03005308 + 1) & 0x3ff;
      r = (*(&gUnk_0203B400[gUnk_03005308])).a;
      sub_081263E0(&v, 128, 32, 12, 12, 24, r, 32, 32, 1, 1, 512);
    }
    if ((*((s32 *) (p + 0x18))) == 12)
    {
      sub_08203660(p);
    }
  }
  return 0;
}
