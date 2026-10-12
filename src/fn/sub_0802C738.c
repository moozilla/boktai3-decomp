// MATCHING_CONCESSION: register-pinning
#include "global.h"
struct Object {
  u8 pad0[0x50];
  u32 flags;
  u8 pad54[0xc0];
  void (*update)(void *);
  void (*destroy)(void *);
  u8 pad11c[0x2b];
  u8 slot;
  u8 pad148[0xd0];
  struct Object *next, *prev;
};
struct Manager {
  u8 pad0[0x18];
  void *targets[4];
  struct Object *head;
  u32 ticks, slot, serial;
};
extern struct Manager *gUnk_020000D8;
extern u8 *gUnk_02000580[4];
int sub_0802C6C8(struct Manager *, struct Object *);
void sub_08219D38(void *);
void sub_0802C710(struct Manager *);
int sub_0802C738(struct Manager *m) {
  struct Object *p = m->head;
  if (p) {
    u32 mask = 1;
    asm("" : "+r"(mask));
    do {
      struct Object *next = p->prev;
      if (p->flags & mask) {
        if (p->destroy)
          p->destroy(p);
        sub_0802C6C8(m, p);
        sub_08219D38(p);
      } else if (p->update)
        p->update(p);
      p = next;
    } while (p);
  }
  m->ticks++;
  m->slot = m->ticks & 3;
  return 0;
}
