#include "global.h"
struct Vm {
  u8 pad0[0x38];
  u8 text[64];
  u8 pad78[0x98];
  const u8 *strings[16];
};
int sub_08032168(const u8 *, const u8 *, int);
int sub_080321A0(const u8 *, int);
int sub_08032D10(struct Vm *, int);
u16 sub_08032070(const u8 *);
const u8 *Text_FindChar(const u8 *, u8);
extern const u8 gUnk_0824DDF4[];
// MATCHING_CONCESSION: register-pinning
int sub_08032D8C(struct Vm *arg, int index) {
  register struct Vm *p asm("r8") = arg;
  register const u8 *s asm("r5") = p->strings[index];
  int n = 0;
  u32 c;
  asm("" : "+r"(p), "+r"(s), "+r"(n));
  goto test;
loop:
  asm("" ::: "memory");
  c = *s;
  if (c == '<') {
    register const u8 *q asm("r4");
    s++;
    if (!*s)
      goto done;
    q = s;
    if (!sub_08032168(s, gUnk_0824DDF4, 3)) {
      register int has asm("r6") = 0;
      int length = 0;
      u8 *text;
      c = *s;
      text = p->text;
      if (c == '>')
        goto parsed;
      if (!c) {
        q = s + 1;
        goto parsed;
      }
      do {
        int ch = *q;
        if (ch == '=')
          has = 1;
        else if (has)
          text[length++] = ch;
        q++;
        c = *q;
        if (c == '>')
          goto parsed;
      } while (c);
      q++;
    parsed:
      text[length] = 0;
      if (has && length)
        index = sub_080321A0(text, length);
      else
        index = 0;
      n += sub_08032D10(p, index);
      Text_FindChar(q, '>');
    } else
      Text_FindChar(s, '>');
    if (!*s)
      goto done;
    goto test;
  }
  if (c == 31) {
    register u32 code asm("r1") = (u16)sub_08032070(s);
    asm("" : "+r"(code));
    if (code == 0xFFFF)
      goto wide;
    {
      register u32 mask asm("r0") = 0xFF;
      asm("" : "+r"(mask));
      mask <<= 8;
      asm("" : "+r"(mask));
      if (mask & code)
        goto wide;
    }
    n++;
    goto wideNext;
  }
  n++;
  s++;
  goto test;
wide:
  n += 2;
wideNext:
  s += 2;
test:
  c = *s;
  c <<= 24;
  asm("" : "+r"(c));
  if ((s32)c < 0)
    goto wide;
  c >>= 24;
  if (!c)
    goto done;
  if (c != 10)
    goto loop;
done:
  return n;
}
