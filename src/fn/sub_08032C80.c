#include "global.h"
struct TextState {
  u8 x, y, left, top, width, height, palette, style;
  u8 entered, mode, skip, busy, padC, choiceCount, padE, fresh, pad10, voice,
      toggle, lastWidth, pad14;
  s8 current;
  u8 instant, wrap;
  u32 frame;
  const u8 *pc, *streams[4];
  u8 pad30[0xD8];
  u8 depth, styles[7];
};
void sub_08032C54(struct TextState *, int, int, int, int);
void sub_08032C34(struct TextState *);
void sub_08032C6C(struct TextState *);
void sub_08032C80(struct TextState *p, int x, int y, int w, int h) {
  int zero;
  sub_08032C54(p, x, y, w, h);
  sub_08032C34(p);
  sub_08032C6C(p);
  zero = 0;
  p->skip = zero;
  p->busy = zero;
  p->padC = p->mode;
  p->lastWidth = zero;
  p->choiceCount = zero;
  p->pc = 0;
  p->streams[0] = 0;
  p->streams[1] = 0;
  p->streams[2] = 0;
  p->streams[3] = 0;
  p->current = -1;
  p->pad14 = 1;
  p->frame = zero;
  p->wrap = zero;
}
