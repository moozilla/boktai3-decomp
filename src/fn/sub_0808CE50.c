#include "global.h"
void sub_08089340(void);
void sub_08020D68(u8 *, u32);
struct S
{
  u8 pad[0x9C];
  s16 z;
  u8 pad2[0xDC - 0x9E];
  u8 extra[0x10];
  u8 pad3[0x104 - 0xEC];
  u32 at104;
  u8 pad4[0x114 - 0x108];
  u32 flags114;
  u8 pad5[0x21C - 0x118];
  void (*callback)(void);
  u8 pad6a[0x22C - 0x220];
  void (*next)(void);
  u8 pad6[0x2A8 - 0x230];
  u8 a;
  u8 b;
  u8 c;
  u8 pad7;
  u32 count;
  u8 done;
  u8 queued;
  u8 taken;
  u8 pad8[2];
  u8 code;
  u8 pad9[0x2C8 - 0x2B6];
  u16 timer;
  u8 pad10[0x2D0 - 0x2CA];
  u32 flags;
  u16 hflags;
  u8 pad11[0x3D0 - 0x2D6];
  struct S *other;
  u32 at3D4;
};
inline static u8 TakeFlag(u8 *p)
{
  if (*p)
  {
    *p = 0;
    return 1;
  }
  return 0;
}

void sub_0808CE50(struct S *s)
{
  void (*new_var)(void);
  s32 off;
  if (TakeFlag(&s->queued))
  {
    void (*fn)(void) = sub_08089340;
    u8 code = 0x23;
    u32 zero;
    u32 *flags;
    s32 mask;
    {
      u8 *taken = &s->taken;
      zero = 0;
      *taken = 1;
    }
    do
    {
      u32 field = 0x2B0;
      do
      {
        ((u8 *) s)[field] = zero;
      }
      while (0);
      field += 5;
      ((u8 *) s)[off = field] = code;
      s->callback = fn;
      field += 0x13;
      *((u16 *) (((u8 *) s) + field)) = zero;
    }
    while (0);
    flags = &s->flags114;
    mask = -2;
    *flags &= mask;
    s->at104 = zero;
  }
  if (s->done)
  {
    void (*fn)(void);
    u32 zero;
    u8 one;
    u8 zero8;
    u32 *flags;
    u32 two;
    sub_08020D68(((u8 *) s) + 0xC, 1);
    off=0x22C; fn=*(void(**)(void))((u8*)s+off);
    zero=0;
    {
      u8 *taken = &s->taken;
      one = 1;
      *taken = one;
    }
    s->done = zero;
    new_var = fn;
    s->code = zero;
    s->callback = new_var;
    zero8 = 0;
    s->timer = zero;
    flags = &s->flags114;
    {
      s32 mask = -2;
      *flags &= mask;
    }
    s->at104 = zero;
    two = 2;
    s->a = two;
    (*(&s))->b = zero8;
    s->c = two;
    s->count = zero;
    s->queued = one;
  }
  else
  {
    ++s->count;
  }
}
