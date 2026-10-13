// MATCHING_CONCESSION: register-pinning
#include "global.h"
void *sub_0803AD30(void);
void *sub_08219FBC(int, int);
void sub_0821A04C(void *, void *, void *);
int sub_0803B370(void *);
int sub_0803B3B4(void *);
int sub_0803B40C(void *);
void sub_0821A0C0(void *);
void *sub_0803B6B8(void) {
  register void *result asm("r0") = sub_0803AD30();
  register void *p asm("r4") = result;
  asm("" : : "r"(p));
  if (!p) {
    p = sub_08219FBC(11, 0x930);
    if (p) {
      sub_0821A04C(p, sub_0803B370, sub_0803B3B4);
      if (sub_0803B40C(p) < 0) {
        sub_0821A0C0(p);
        return 0;
      }
    }
    result = p;
  }
  asm("" : "+r"(result));
  return result;
}