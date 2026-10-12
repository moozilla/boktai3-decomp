#include "global.h"
// MATCHING_CONCESSION: register-pinning
void *sub_0802C860(int);
void sub_0802C900(void *, void (*)(void *), void (*)(void *));
void sub_0802D41C(void *);
void sub_0802D43C(void *);
int sub_0802D44C(void *);
void sub_0802C8A0(void *);
void *sub_0802D570(void) {
  register void *p asm("r4") = sub_0802C860(0x220);
  void *allocated = p;
  asm("" : "+r"(p));
  if (p) {
    sub_0802C900(p, sub_0802D41C, sub_0802D43C);
    if (sub_0802D44C(p) >= 0)
      return p;
    sub_0802C8A0(allocated);
  }
  return 0;
}
