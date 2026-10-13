// MATCHING_CONCESSION: register-pinning
#include "global.h"
struct Panel  {
  u8 pad0[0x32];
  u8 enabled;
  u8 pad33[9];
  u8 values[7];
}
;
struct Panel*sub_08037DC0(void);
void sub_0822B2F8(int);
int sub_08039774(void) {
  struct Panel*p=sub_08037DC0();
  asm("" : "+r"(p));
  if(p&&p->enabled) {
    u8*v=p->values;
    v[6]=1-v[6];
    sub_0822B2F8(267);
    return v[6];
  }
  return 0;
}
