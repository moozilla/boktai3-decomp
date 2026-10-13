// MATCHING_CONCESSION: register-pinning
#include "global.h"
struct PeerMenu  {
  u8 pad0[0x19];
  u8 state,entered,pad1b[5];
  u32 timer;
  u8 pad24[0x4c8];
  int window;
}
;
void sub_08033924(int,int);
void sub_0822B2F8(int);
void sub_08036B14(void);
extern u16 gUnk_03005260[];
void sub_08037730(struct PeerMenu*p) {
  if(p->entered) {
    p->entered=0;
    sub_08033924(p->window,2);
    sub_0822B2F8(402);
  }
  if(p->timer>35) {
    if(gUnk_03005260[1]&3) {
      sub_08036B14();
      {
        register u32 state asm("r0")=7;
        register u32 zero asm("r1")=0;
        asm("" : "+r"(state),"+r"(zero));
        p->state=state;
        p->entered=1;
        p->timer=zero;
      }
    }
  }
  p->timer++;
}
