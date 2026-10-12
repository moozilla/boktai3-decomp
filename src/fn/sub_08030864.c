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
void *sub_08219FBC(int, int);
void sub_0821A04C(void *, void *, void *);
int sub_08030818(struct Manager *);
void sub_0821A0C0(void *);
void sub_08030710(void);
void sub_080307D8(void);
struct Manager *sub_08030864(void) {
  struct Manager *p;
  if (gUnk_02000480)
    return gUnk_02000480;
  p = sub_08219FBC(9, 0x43c);
  if (p) {
    sub_0821A04C(p, sub_08030710, sub_080307D8);
    if (sub_08030818(p) < 0) {
      sub_0821A0C0(p);
      return 0;
    }
  }
  return p;
}
