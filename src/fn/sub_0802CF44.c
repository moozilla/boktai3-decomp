#include "global.h"
struct Object {
  u8 pad0[0xc];
  s16 pos[4];
  u8 pad14[0x3c];
  u32 flags, transient;
  u8 pad58[0xc4];
  void (*callback)(void *, int);
  u8 pad120[0x14];
  int ticks;
  u8 pad138[0x49];
  u8 facing;
  u8 pad182[2];
  u32 drawable;
};
void sub_0802CB94(struct Object *);
void sub_080426B0(void *, void *);
void sub_080423C8(void *);
void sub_0802CF44(struct Object *p) {
  int tick;
  p->transient = 0;
  tick = p->ticks;
  p->ticks = tick + 1;
  if (p->callback)
    p->callback(p, tick);
}
