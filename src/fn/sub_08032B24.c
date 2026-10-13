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
const u8 *sub_08032A44(struct TextState *, const u8 *);
void sub_08031E84(struct TextState *);
int sub_08032B24(struct TextState *p) {
  const u8 *s;
  int result;
  int c;
  p->padE = 0;
  switch (p->style) {
  case 0:
    s = sub_08032A44(p, p->pc);
    p->pc = s;
    if (p->fresh)
      goto yes;
    if (p->style)
      goto no;
    c = *s;
    goto endchar;
  case 1:
    s = sub_08032A44(p, p->streams[0]);
    p->streams[0] = s;
    if (p->style != 1)
      goto no;
    goto stream;
  case 2:
    s = sub_08032A44(p, p->streams[1]);
    p->streams[1] = s;
    if (p->style != 2)
      goto no;
    goto stream;
  case 3:
    s = sub_08032A44(p, p->streams[2]);
    p->streams[2] = s;
    if (p->style != 3)
      goto no;
    goto stream;
  case 4:
    s = sub_08032A44(p, p->streams[3]);
    p->streams[3] = s;
    if (p->style != 4)
      goto no;
  stream:
    if (*s)
      goto no;
    sub_08031E84(p);
    if (p->style)
      goto no;
    if (p->fresh)
      goto yes;
    c = *p->pc;
  endchar:
    if (c)
      goto no;
    goto yes;
  default:
    goto no;
  }
yes:
  result = 1;
  goto done;
no:
  result = 0;
done:
  return result;
}
