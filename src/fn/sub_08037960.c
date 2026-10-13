#include "global.h"
struct Row  {
  u8 pad0[0x58];
  u32 flags;
  u8 pad5c[4];
}
;
struct PeerMenu  {
  u8 pad0[0x1a];
  u8 entered,pad1b[5];
  u32 timer;
  u8 pad24[0x264];
  struct Row rows[5];
  u8 pad468[0x84];
  int window;
}
;
void sub_08033924(int,int);
void sub_08037960(struct PeerMenu*p) {
  if(p->entered) {
    struct Row*r;
    int i;
    u32 mask;
    p->entered=0;
    r=p->rows;
    sub_08033924(p->window,6);
    mask=1;
    i=4;
    do {
      r->flags|=mask;
      r++;
    }
    while(--i>=0);
  }
  p->timer++;
}
