#include "global.h"
int sub_0816B59C(void *, int);
void sub_0816D920(void *, int);
void sub_0816D868(void *);
void sub_08166B7C(void *, int);
void sub_0816DAF8(void *);
void sub_0822B2F8(int);
void sub_0816B614(void *);
void sub_081704B4(void);
void sub_08163EB8(void *, void (*)(void), int);
extern u16 gUnk_03005260[];
void sub_0817040C(u8 *p)
{
  u8 *q = p + 0x4384;
  if (sub_0816B59C(q, 0xf0))
  {
    u8 *a = p + 0x1620;
    sub_0816D920(a, p[0x4387]);
    sub_0816D868(p);
  }
  sub_08166B7C(p, 4);
  if (gUnk_03005260[1] & 2)
  {
    sub_0816DAF8(p);
    sub_0822B2F8(0xde);
  }
  else
    if (1 & gUnk_03005260[1])
  {
    u8 *a;
    sub_0822B2F8(0xdd);
    sub_0816B614(q);
    *((u32 *) (p + 0x1668)) &= ~1;
    a = p + 0x1680;
    sub_0816D920(a, p[0x438a]);
    sub_08163EB8(p, sub_081704B4, 1);
  }
}
