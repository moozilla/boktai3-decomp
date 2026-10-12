#include "global.h"
struct Actor {
  u8 pad0[0xe];
  u16 flags;
  u8 pad10[9];
  u8 facing4, facing8;
  u8 pad1b[0xb9];
  u32 drawable;
};
int sub_08042320(void *, int, int, int, int, int, int);
int sub_0802DC18(struct Actor *p, int a, int b, int stride, int c, int d) {
  int direction, flip;
  if (p->drawable == 1) {
    if (p->facing8 > 4) {
      direction = 8 - p->facing8;
      flip = 1;
    } else {
      direction = p->facing8;
      flip = 0;
    }
  } else {
    if (p->facing8 > 4) {
      direction = 8 - p->facing8;
      flip = 1;
    } else {
      direction = p->facing8;
      flip = 0;
    }
    p->facing8 = p->facing8 * stride;
  }
  return sub_08042320(&p->drawable, a, direction, flip, b, c, d);
}
