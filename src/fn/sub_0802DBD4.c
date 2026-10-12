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
int sub_0802DBD4(struct Actor *p, int a, int b, int c, int d, int e) {
  void *draw = &p->drawable;
  u32 direction, flip;
  if (p->drawable == 1) {
    direction = p->facing4;
    flip = 0;
    if (direction > 1)
      flip = 1;
  } else {
    u32 facing;
    direction = 0;
    facing = p->facing4;
    flip = 0;
    if (facing > 1)
      flip = 1;
  }
  return sub_08042320(draw, a, direction, flip, b, d, e);
}
