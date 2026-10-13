#include "global.h"
struct Common {
  u8 pad0[0x90];
  u32 timer;
  s16 health, maxHealth;
  u8 pad98[2];
  u8 healthCheck, pad9b;
  u32 flags;
  u8 padA0[2];
  u8 reset;
  u8 padA3;
  u32 pending;
  u8 facing, facing8, requested, state;
  u8 active, actionReset, action, finished, entered, changed;
  u8 padB2[0x12];
  u32 actionTimer;
  u8 padC8[0x38];
  void (*callback)(void *);
};
static inline void state(struct Common *p, void (*fn)(void *), int id) {
  int zero;
  p->state = id;
  zero = 0;
  p->callback = fn;
  p->timer = zero;
  p->changed = 1;
  p->reset = zero;
  p->pending = zero;
}
int sub_0802AD0C(void *);
void sub_0802A63C(void *);
static inline u8 chk(struct Common *p) {
  if (p->entered) {
    p->entered = 0;
    p->finished = 0;
    return 1;
  }
  return 0;
}
static inline void action(struct Common *p, int id, int zero) {
  int one = 1;
  p->active = one;
  p->actionReset = zero;
  p->action = id;
  p->actionTimer = zero;
  p->entered = one;
}
void sub_0802B530(struct Common *p) {
  struct Common *base = p;
  p->healthCheck = 1;
  if (chk(p)) {
    u32 id = 4;
    p->requested = id;
  }
  if (p->finished) {
    int duration, zero;
    int value = *((u8 *)p + 0xe1);
    if (value < 64)
      duration = *((u8 *)base + 0x1b7);
    else
      duration = *((u8 *)base + 0x1b6);
    zero = 0;
    base->active = zero;
    base->actionReset = zero;
    base->action = zero;
    base->actionTimer = zero;
    base->entered = 1;
    *(u16 *)((u8 *)base + 0x190) = duration;
  } else
    p->actionTimer++;
}
