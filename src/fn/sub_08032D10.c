// MATCHING_CONCESSION: register-pinning
#include "global.h"
struct Vm {
  u8 pad[0x88];
  s32 numbers[16];
};
int sub_08032D10(struct Vm *p, int index) {
  register int value asm("r1") = p->numbers[index];
  register u32 n asm("r2") = (u32)value >> 31;
  register int abs asm("r0") = value;
  asm("" : "+r"(n), "+r"(abs), "+r"(value));
  if (value < 0)
    abs = -value;
  value = abs;
  asm("" : "+r"(value));
  if (value > 9999999)
    n += 8;
  else if (value > 999999)
    n += 7;
  else if (value > 99999)
    n += 6;
  else if (value > 9999)
    n += 5;
  else if (value > 999)
    n += 4;
  else if (value > 99)
    n += 3;
  else if (value > 9)
    n += 2;
  else
    n++;
  return n;
}
