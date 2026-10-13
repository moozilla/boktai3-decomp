#include "global.h"
// MATCHING_CONCESSION: register-pinning
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
  u8 frame;
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
int sub_08030710(struct Manager *m) {
  struct Player *player = gUnk_02000580[0];
  int zero;
  m->player = player;
  if (player)
    m->target = &player->pos;
  else
    m->target = 0;
  m->event = sub_08018420();
  m->phase = sub_08228D88();
  {
    int frame = sub_08228D94();
    u8 *current = &m->frame;
    register u8 *previous asm("r1");
    register int old asm("r3");
    u8 *keep;
    *current = frame;
    previous = &m->previous;
    old = *previous;
    frame = (u8)frame;
    keep = previous;
    if (old != frame)
      *(previous - 1) = 1;
    else
      m->changed = 0;
    {
      int value = *current;
      zero = 0;
      *keep = value;
    }
  }
  {
    u32 buffer = 1 - m->buffer;
    m->buffer = buffer;
    m->count[buffer] = zero;
  }
  {
    struct Actor *p = m->head;
    while (p) {
      struct Actor *prev = p->prev;
      sub_0802D9C0(m, p);
      p = prev;
    }
  }
  sub_0802E188(m);
  if (m->player)
    sub_0802E04C((u8 *)m->player + 0x24);
  m->ticks++;
  return 0;
}
