#include "global.h"
struct Vec {
  s16 x, y, z, pad;
};
struct Actor {
  u8 pad0[7];
  u8 done;
  u8 pad8[1];
  u8 entered;
  u8 pada[4];
  u16 flags;
  u8 pad10[8];
  u8 phase;
  u8 facing4;
  u8 facing8;
  u8 targetAngle;
  u8 pad1c[56];
  struct Vec target;
  u8 body[9];
  u8 angle;
  u8 pad66[2];
  struct Vec pos;
  struct Vec velocity;
  u8 pad78[92];
  u32 drawable;
  u8 padd8[80];
  struct Vec *sourcePos;
  u8 pad12c[52];
  struct Vec *drawPos;
  u8 pad164[0x88];
  u8 modeA, modeB;
  u8 pad1ee[2];
  u8 cap;
  u8 pad1f1[0x17];
  int speed;
  u32 speedSquared;
  u8 pad210[212];
  int (*callback)(struct Actor *);
};
struct Map {
  u32 pad;
  u8 *cells;
  u8 pad8[28];
  u16 rows[1];
};
extern struct Map *gUnk_030052F4;
extern u32 gUnk_030052F8, gUnk_030052FC;
u8 *sub_0821D64C(int, int);
extern const s16 gUnk_086149C4[];
int sub_0802DCD8(struct Actor *, int, int, int, int);
void sub_0802DD6C(struct Actor *, int);
void sub_080426D4(void *, int);
void sub_080426F8(void *, int);
void sub_0804275C(void *, int, int, int);
void sub_0822B2F8(int);
int sub_082215E4(int, int);
static inline int scale(int v) {
  if (v >= 0)
    return v >> 12;
  return -((-v) >> 12);
}
static inline u8 *cell(int i) {
  u8 *p = sub_0821D64C(i, 1);
  if (p)
    return p + 4;
  return gUnk_030052F4->cells + 12 + i * 4;
}
static inline int height(struct Vec *pos) {
  int x = pos->x >> 8, z = pos->z >> 8, index;
  if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
    index = 0;
  else
    index = gUnk_030052F4->rows[z] + x;
  return (*cell(index) & 15) << 8;
}
void sub_0802F524(void *unused, struct Actor *p, int ticks) {
  if (p->entered) {
    int zero = 0;
    p->entered = zero;
    p->done = zero;
    sub_0802DCD8(p, 0, 1, zero, 4);
    p->cap = 1;
  }
  {
    u32 mask = 4096;
    p->flags |= mask;
  }
  if ((u8)p->callback(p) != 1 && ticks > 7)
    p->done = 1;
}
