#include "global.h"
// MATCHING_CONCESSION: register-pinning
// Tentative: Turn toward the selected target by at most four angular units.
struct Vec {
  s16 x, y, z, pad;
};
struct Object {
  u8 pad0[9];
  u8 angle;
  u8 pada[2];
  struct Vec pos;
  u8 pad14[0x124];
  u32 range;
  u8 arc, pad13d[3];
  u32 distance;
  u8 delta;
  s8 target;
  u8 count, slot;
};
void *sub_0802C678(void);
struct Object *sub_0802CA8C(int);
int sub_082215E4(int, int);
void sub_0802C8CC(struct Object *, int);
void sub_0802CC94(struct Object *p) {
  if (p->target >= 0) {
    register s8 *deltaPtr asm("r3") = (s8 *)&p->delta;
    int delta, angle;
    asm("" : "+r"(deltaPtr));
    delta = *deltaPtr;
    if (delta > 3)
      angle = p->angle + 4;
    else if (delta <= -4)
      angle = p->angle - 4;
    else {
      asm("" : "+r"(deltaPtr) : : "memory");
      angle = *deltaPtr + p->angle;
    }
    sub_0802C8CC(p, angle);
  }
}
