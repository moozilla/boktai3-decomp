#include "global.h"
struct TextState {
  u8 x, y, left, top, width, height, palette, style;
  u8 entered, mode, skip, busy, padC, choiceCount, padE, fresh, pad10, voice,
      toggle, lastWidth, pad14;
  s8 current;
  u8 instant, wrap;
  u8 pad18[4];
  const u8 *pc;
  u8 pad20[0xE8];
  u8 depth, styles[7];
};
void sub_08031E54(struct TextState *p, int style) {
  u8 *depth = &p->depth;
  if (*depth <= 6) {
    p->styles[*depth] = p->style;
    (*depth)++;
    p->style = style;
  }
}
