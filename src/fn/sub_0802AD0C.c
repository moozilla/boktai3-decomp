#include "global.h"
struct Common {
  u8 pad0[0x90];
  u32 timer;
  s16 health, maxHealth;
  u8 pad98[4];
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
int sub_08048390(void *);
int sub_08046870(void *);
void sub_08046834(void *);
static inline void setAction(struct Common *p, int id, int zero) {
  p->action = id;
  p->actionTimer = zero;
  p->entered = 1;
}
int sub_0802AD0C(struct Common *p) {
  struct Common *base = p;
  int trigger = *((u8 *)p + 0x1a2);
  if (trigger) {
    int zero = 0;
    *((u8 *)p + 0x1a2) = zero;
    setAction(p, 2, zero);
    return 1;
  } else {
    int active = sub_08048390(*(void **)((u8 *)p + 0x90c));
    if (active) {
      setAction(p, 3, 0);
    } else if (*(s16 *)((u8 *)p + 0x1a0) >= *(s16 *)((u8 *)p + 0x1a4)) {
      if (sub_08046870(*(void **)((u8 *)p + 0x904)) > 0)
        sub_08046834(*(void **)((u8 *)p + 0x904));
      setAction(p, 1, 0);
    } else {
      setAction(base, 3, 0);
    }
  }
  return 0;
}
