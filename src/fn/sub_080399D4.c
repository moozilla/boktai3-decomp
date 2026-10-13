#include "global.h"
struct Item {
  u8 state,entered,pad2[0x22];
  u32 flags;
  u8 pad28[0x30];
  u32 flags2;
}
;
void sub_080399D4(void*p,struct Item*q) {
  if(q->entered) {
    q->entered=0;
    q->flags|=1;
    q->flags2|=1;
  }
}
