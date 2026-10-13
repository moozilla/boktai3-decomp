#include "global.h"
struct Draw {
  u8 pad[8];
  u32 flags;
  u8 tail[0x54];
};
struct StatusMenu {
  u8 task[0x18];
  u32 timer;
  u8 state, entered, flag, pad1f;
  u8 pad20[0x20];
  struct Draw draws[4];
};
int sub_0803AC9C(struct StatusMenu *p, int state) {
  struct Draw *d;
  int i;
  u32 mask;
  p->state = state;
  p->entered = 1;
  p->timer = 0;
  p->flag = 0;
  d = p->draws;
  mask = 1;
  i = 3;
  do {
    d->flags |= mask;
    i--;
    d++;
  } while (i >= 0);
  return 0;
}
