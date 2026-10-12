#include "global.h"
struct Vec {
  s16 x, y, z, pad;
};
struct Player {
  u8 pad[0x30];
  struct Vec pos;
};
struct Actor {
  u8 pad[0x2e0];
  void (*destroy)(void *, struct Actor *);
  u8 pad2e4[8];
  struct Actor *prev;
};
struct Manager {
  u8 pad0[0x18];
  u32 ticks;
  struct Actor *head;
  u8 changed, previous, phase;
  volatile u8 frame;
  u8 event, pad25[3];
  u32 buffer, count[2];
  u8 pad34[0x400];
  struct Vec *target;
  struct Player *player;
};
extern struct Manager *gUnk_02000480;
extern struct Player *gUnk_02000580[4];
int sub_08018420(void);
int sub_08228D88(void);
int sub_08228D94(void);
void sub_0802D9C0(void *, struct Actor *);
void sub_0802E188(struct Manager *);
void sub_0802E04C(void *);
int sub_0802D96C(struct Actor *);
int sub_080307D8(struct Manager *m) {
  struct Actor *p = m->head;
  while (p) {
    struct Actor *prev = p->prev;
    if (p->destroy)
      p->destroy(m, p);
    sub_0802D96C(p);
    p = prev;
  }
  return (gUnk_02000480 = 0);
}
