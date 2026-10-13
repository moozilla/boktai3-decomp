#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct State {
  u8 pad0[15];
  u8 fresh;
  u8 pad10[5];
  s8 current;
  u8 pad16[6];
  void *pc;
};
struct Render {
  u32 flags;
  u8 pad[12];
  u16 tile;
};
struct Frames {
  u16 *data;
  u8 flags, count, multiplier, duration;
  u16 index, pada, rate, timer;
};
struct Actor {
  u8 pad0[0x18];
  struct State state;
  u8 pad38[0x144];
  u8 x, y, width, height;
  u16 scriptIndex;
  u8 mode, pad183, previousMode, pad185[2], defaultMode;
  u8 pad188[14];
  u16 wait;
  u8 pad198[4];
  u32 counter;
  u8 pad1a0[4];
  void *script;
  u16 scriptFrame, scriptCount;
  u8 pad1ac[0x6c];
  u32 flags;
  u8 pad21c[0x54];
  u8 sprite[0x1c];
  struct Render render;
  u8 pad2a0[0x18];
  struct Frames frames;
  void *resource;
  void *callback;
};
extern struct Actor *gUnk_020000DC;
int sub_0803092C(struct Actor *);
void sub_080316A4(void);
void sub_080317D8(void);
void sub_080317CC(void);
int sub_08218E5C(int, int, int, int);
void *sub_080313C8(struct Actor *, int);
void sub_08032C34(struct State *);
int sub_08032C54(struct State *, int, int, int, int);
int Script_GetValue(void);
static inline void setCallback(void *fn, struct Actor *p) { p->callback = fn; }
static inline void setCallbackB(struct Actor *p, void *fn) { p->callback = fn; }
static inline u8 getMode(struct Actor *p, u32 offset) {
  return *((u8 *)p + offset);
}
int sub_08030DBC(int index) {
  struct Actor *p = gUnk_020000DC;
  if (p) {
    void *script = p->script;
    struct State *state = &p->state;
    asm("" : "+r"(state) : "r"(script) : "r1");
    if (script) {
      sub_08218E5C(p->x, p->y, p->width, p->height);
      sub_08032C54(state, p->x, p->y, p->width, p->height);
      p->scriptIndex = index;
      {
        void *pc = sub_080313C8(p, p->scriptIndex);
        register struct State *dst asm("r3") = state;
        dst->pc = pc;
      }
      {
        u8 mode;
        {
          register u32 offset asm("r1") = 0x182;
          register u8 *ptr asm("r0");
          ptr = (u8 *)p + offset;
          mode = *ptr;
        }
        if (!mode) {
          {
            register void *fn asm("r0") = sub_080316A4;
            register void **dst asm("r1");
            register u32 offset asm("r2") = 0x2cc;
            dst = (void **)((u8 *)p + offset);
            *dst = fn;
          }
          {
            register u32 offset asm("r3") = 0x187;
            register u8 *mode asm("r1");
            mode = (u8 *)p + offset;
            *mode = 3;
          }
        } else {
          {
            register u32 offset asm("r0") = 0x19c;
            u32 *counter;
            counter = (u32 *)((u8 *)p + offset);
            {
              register u32 zero asm("r0") = 0;
              *counter = zero;
            }
          }
          {
            register void *fn asm("r1") = sub_080317D8;
            register u32 offset asm("r2");
            register void **dst asm("r0");
            offset = 0x2cc;
            dst = (void **)((u8 *)p + offset);
            *dst = fn;
          }
        }
      }
      sub_08032C34(state);
      {
        register u32 offset asm("r3") = 0x196;
        register u16 *wait asm("r1");
        wait = (u16 *)((u8 *)p + offset);
        {
          u32 value = 65535;
          *wait = value;
        }
      }
      {
        register u32 offset asm("r0") = 0x218;
        u32 *flags;
        flags = (u32 *)((u8 *)p + offset);
        {
          register u32 value asm("r0") = *flags;
          u32 mask = 1;
          value |= mask;
          *flags = value;
        }
      }
    }
  }
  return 0;
}
