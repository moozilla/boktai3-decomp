#include "global.h"
struct Menu  {
  u8 pad0[0x28];
  u8 state,entered,pending;
  u8 pad2b[0x29];
  u8 message[16];
}
;
int sub_08036BEC(void);
void sub_0824923C(void*,void*);
int sub_08036B64(void*);
extern void*gUnk_086057A0[];
int sub_08039058(struct Menu*p) {
  int busy=sub_08036BEC();
  if(!busy) {
    sub_0824923C(p,gUnk_086057A0[p->state]);
    if(p->pending) {
      if(sub_08036B64(p->message)>=0)p->pending=busy;
    }
  }
  return 0;
}
