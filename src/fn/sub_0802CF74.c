#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct Collision {
  u8 pad[6];
  u16 flags;
};
struct Object {
  u8 pad0[0x50];
  u32 flags;
  u8 pad54[0x10];
  u16 effectTimer, collisionTimer;
  u32 effect;
  u8 sensor[0x54];
  struct Collision collider;
  u8 padc8[0x7f];
  u8 slot;
  u8 pad148[0x3c];
  u32 drawable;
};
struct Manager {
  u8 pad[0x30];
  u32 slot;
};
struct Manager *sub_0802C678(void);
void sub_0821FE40(void *);
void sub_08042798(void *, u32);
void sub_0802C994(struct Object *);
static inline void enable(struct Object *p) {
  register struct Collision *c asm("r2") = &p->collider;
  asm("" : "+r"(c));
  {
    register u32 old asm("r1") = c->flags;
    register u32 result asm("r0");
    asm("" : "+r"(old));
    result = old | 4;
    asm("" : "+r"(result));
    c->flags = result;
  }
}
static inline void disable(struct Object *p) {
  struct Collision *c = &p->collider;
  int mask = ~4;
  asm("" : "+r"(c), "+r"(mask));
  c->flags &= mask;
}
void sub_0802CF74(struct Object *p) {
  struct Manager *m = sub_0802C678();
  int selected = 0;
  if (m->slot == p->slot)
    selected = 1;
  if (selected) {
    u32 mask = 4;
    if (p->flags & mask)
      sub_0821FE40(p->sensor);
  }
  if (!p->collisionTimer) {
    u32 mask = 8;
    if (p->flags & mask)
      disable(p);
    else
      enable(p);
  } else {
    enable(p);
    p->collisionTimer--;
  }
  if (p->effectTimer) {
    p->effectTimer--;
    if (!p->effectTimer)
      sub_08042798(&p->drawable, p->effect);
  }
  sub_0802C994(p);
}
