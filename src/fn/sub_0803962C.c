// MATCHING_CONCESSION: register-pinning
#include "global.h"
struct Panel  {
  u8 pad0[0x3c];
  u8 counts[4];
  u8 pad40[0xaa];
  u8 choice, counter;
}
;
struct Panel *sub_08037DC0(void);
void sub_0803A514(void *, int, int);
void sub_080397D4(int, int);
void sub_0822B2F8(int);
int sub_0803962C(void *owner)  {
  struct Panel *p = sub_08037DC0();
  int index;
  u8 *choice, *counts;
  if (!p) goto unavailable;
  {
    if (!++p->counter)
    {
      register int mask asm("r0")=255;
      asm("" : "+r"(mask));
      p->counter |= mask;
    }
    if (p->counter != 1)  {
      unavailable:return 0;
    }
    {
      {
        register u8*field asm("r0")=&p->choice;
        register int value asm("r2");
        asm("" : "+r"(field));
        value=*field;
        choice=field;
        asm("" : "+r"(value));
        switch (value)  {
          case 8: index = 3;
          break;
          case 7: index = 1;
          break;
          case 6: default: index = 2;
          break;
        }
      }
      counts = p->counts;
      if (counts[index] <= 4)  {
        int total = 0, i = 0;
        do  {
          total += counts[i];
        }
        while (++i <= 3);
        if (total <= 9)  {
          sub_0803A514(owner, index, 1);
          counts[index]++;
          sub_080397D4(2, 1);
          sub_0822B2F8(885);
          if (++*choice > 8)
          *choice = 6;
        }
      }
      return 1;
    }
  }
}
