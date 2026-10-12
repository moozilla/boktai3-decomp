#include "global.h"
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
  u8 mode, pad183, previousMode, pad185, scrollMode, defaultMode;
  u16 scrollStep, scrollCounter;
  void *custom;
  u32 customCount;
  u16 palette, wait;
  u32 scrollFlags;
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
struct Vm {
  u8 pad0[9];
  u8 mode;
  u8 pada[3];
  u8 choiceCount;
  u8 pade[3];
  u8 spacing;
  u8 pad12[0xb6];
  u32 choices[16];
};
struct Sun {
  u8 pad[0x12];
  s16 level;
};
extern struct Sun *gUnk_02000710;
int Script_SeekToKeyword(int);
void *sub_08227E90(void);
void *Script_GetPc(void);
int sub_0821ABA8(int, int);
int sub_080308D0(struct Actor *, int);
void sub_08030908(struct Actor *);
int sub_08030C84(void *);
int sub_08030F20(int, int, int, int);
int Script_ParseStringRef(void *);
void *Text_LookupString(int);
int sub_08031018(int, void *);
int sub_08031144(int, int, int);
int sub_08031184(void) {
  int mode = Script_GetValue();
  int step = Script_GetValue();
  int flags = sub_0821ABA8(0x6d, mode != 0);
  return sub_08031144(mode, step, flags);
}
