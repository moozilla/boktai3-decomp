// MATCHING_CONCESSION: register-pinning
#include "global.h"
struct Blob {
  u32 words[8];
};
struct Sprite {
  u32 flags;
  u8 pad4[3], visible;
  u8 pad8[8];
  u16 tile;
  u8 pad12[0x1a];
};
struct Animation {
  const u16 *frames;
  u8 flags, length, duration, period;
  u16 frame, base, speed, timer;
};
struct Drawing {
  u8 bytes[0x60];
};
struct ResultMenu {
  u8 task[0x18];
  u32 timer;
  u8 state, entered;
  u8 pad1e[0x12];
  struct Blob primary, secondary;
  struct Sprite sprite;
  u8 glyph[0x1c];
  struct Drawing draws[22];
  void *onClose, *resource;
  int windows[4];
  u8 left[16], right[16];
};
extern struct ResultMenu *gUnk_02000104;
int Script_SeekToKeyword(int);
void *sub_08227E90(void);
void *sub_0821A520(int, int);
void sub_082196C4(void *, void *);
void sub_0821983C(void *, void *, int, int, int, int, int, void *);
void sub_082151E4(void *, int);
void sub_08215284(void *, int);
void sub_082144A4(void *, void *, int);
void sub_08220D20(void *, void *, int, int, int);
void *sub_08037DCC(int);
void sub_08219EBC(void *, void *, int);
int sub_0803B40C(struct ResultMenu *arg) {
  register struct ResultMenu *p asm("r9") = arg;
  register struct Blob *res asm("r2");
  register struct Blob *first asm("r4");
  register int pack asm("r5");
  register struct Blob *primary asm("r10");
  register struct Blob *secondary asm("r5");
  int i;
  register void *resource asm("r4");
  register void *glyph asm("r6");
  struct Saved {
    struct Animation animation;
    u8 *visible;
  } saved;
  register struct Animation *a asm("r5");
  register const u16 *frame asm("r6");
  struct Sprite *sprite;
  register struct Sprite *savedSprite asm("r8");
  int flags;
  gUnk_02000104 = p;
  if (Script_SeekToKeyword(115)) {
    void *value = sub_08227E90();
    register void **slot asm("r1") = &p->resource;
    asm("" : "+r"(slot));
    *slot = value;
    if (value)
      goto initialize;
  }
failed:
  return -1;
initialize:
  pack = 0xCB05;
  asm("" : "+r"(pack));
  res = sub_0821A520(pack, 0x530D);
  if (!res)
    goto failed;
  asm("" : "+r"(res));
  p->primary = *res;
  first = &p->primary;
  sub_082196C4(first, res);
  res = sub_0821A520(pack, 0x7C8B);
  primary = first;
  if (!res)
    goto failed;
  asm("" : "+r"(res));
  p->secondary = *res;
  secondary = &p->secondary;
  sub_082196C4(secondary, res);
  {
    struct Drawing *draw = p->draws;
    register u32 drawFlags asm("r8") = 0x2011;
    sub_0821983C(draw, secondary, 30, drawFlags, 0, 0, 60, 0);
    draw = p->draws + 1;
    sub_0821983C(draw, secondary, 2, drawFlags, 0, 0, 60, 0);
    draw = p->draws + 2;
    glyph = p->glyph;
    savedSprite = &p->sprite;
    {
      register u8 *visible asm("r1") = &p->sprite.visible;
      asm("" : "+r"(visible));
      saved.visible = visible;
    }
    {
      register int zero asm("r5") = 0;
      register int count asm("r4") = 19;
      asm("" : "+r"(zero));
      do {
        sub_0821983C(draw, primary, 51, 0x2011, zero, zero, 60, (void *)zero);
        count--;
        draw++;
      } while (count >= 0);
    }
  }
  resource = sub_0821A520(0x922E, 0x4BBF);
  sub_082151E4(glyph, 0x593A);
  sub_08215284(glyph, 503);
  sub_082144A4(savedSprite, glyph, 0x2011);
  sub_08220D20(&saved.animation, resource, 0, 0, 0);
  sprite = savedSprite;
  asm("" : "+r"(sprite));
  a = &saved.animation;
  {
    register int offset asm("r0") = a->frame << 1;
    register const u16 *table asm("r1");
    asm("" : "+r"(offset));
    table = saved.animation.frames;
    frame = (const u16 *)((u32)table + offset);
  }
  sprite->tile = *frame >> 6;

  // Combine the animation and frame orientation flags for the sprite.
  {
    register int observed asm("r0") = a->flags;
    register int one asm("r3");
    register int side asm("r1");
    register int word asm("r2");
    register u32 encoded asm("r0");
    asm("" : "+r"(observed));
    one = 1;
    side = one;
    asm("" : "+r"(side));
    side &= observed;
    word = *frame;
    encoded = word & 48;
    asm("" : "+r"(encoded));
    encoded >>= 4;
    encoded &= one;
    if (side != encoded) {
      register struct ResultMenu *write asm("r3") = p;
      register int value asm("r0") = write->sprite.flags;
      value |= 4;
      write->sprite.flags = value;
    } else {
      register struct ResultMenu *write asm("r4") = p;
      register int value asm("r0") = write->sprite.flags;
      register int mask asm("r1") = -5;
      value &= mask;
      write->sprite.flags = value;
    }
  }
  {
    register int observed asm("r4") = a->flags;
    register int two asm("r3") = 2;
    register int side asm("r1");
    register int word asm("r2");
    register u32 encoded asm("r0");
    side = two;
    asm("" : "+r"(side));
    side &= observed;
    asm("" : "+r"(side));
    side = (u8)side;
    word = *frame;
    encoded = word & 48;
    asm("" : "+r"(encoded));
    encoded >>= 4;
    encoded &= two;
    if (side != encoded)
      sprite->flags |= 8;
    else
      sprite->flags &= ~8;
    flags = observed;
  }
  {
    register u32 elapsed asm("r0") = a->timer + 1;
    register u32 limit asm("r3");
    register u32 initialPeriod asm("r1");
    register int savedPeriod asm("r6");
    register int bits asm("r2");
    a->timer = elapsed;
    {
      register u32 constant asm("r6") = 65535;
      asm("" : "+r"(constant));
      limit = constant;
    }
    initialPeriod = a->period;
    elapsed = (u16)elapsed;
    savedPeriod = initialPeriod;
    if (elapsed < initialPeriod)
      goto animationDone;
    a->timer = 0;
    bits = flags;
    asm("" : "+r"(bits));
    if (bits & 4) {
      register int previous asm("r0");
      previous = a->frame;
      if (previous)
        goto decrementFrame;
      if (bits & 8) {
        a->timer = savedPeriod;
        goto recalculate;
      }
      previous = a->length;
    decrementFrame:
      previous--;
      a->frame = previous;
      goto recalculate;
    } else {
      register u32 next asm("r0") = a->frame + 1;
      register int length asm("r2");
      a->frame = next;
      length = a->length;
      next &= limit;
      if (next < (u32)length)
        goto recalculate;
      next = flags & 8;
      asm("" : "+r"(next));
      next = (u8)next;
      if (next) {
        a->timer = initialPeriod;
        next = length - 1;
      }
      a->frame = next;
    }
  recalculate: {
    register int index asm("r0") = a->frame << 1;
    register const u16 *table asm("r1");
    register int word asm("r1");
    register int value asm("r0");
    asm("" : "+r"(index));
    table = a->frames;
    frame = (const u16 *)((u32)table + index);
    word = *frame;
    value = word & 15;
    a->duration = value;
    asm("" : : : "memory");
    value = a->duration * a->speed;
    asm("" : "+r"(value));
    value >>= 6;
    a->period = value;
    value = (u32)value << 24;
    asm("" : "+r"(value));
    if (!value)
      a->period = 1;
  }
  animationDone:;
  }

  {
    register int zero asm("r5") = 0;
    asm("" : "+r"(zero));
    {
      register u8 *visible asm("r0") = saved.visible;
      asm("" : "+r"(visible));
      *visible = zero;
    }
    sub_08219EBC(p->left, sub_08037DCC(0), 16);
    sub_08219EBC(p->right, sub_08037DCC(1), 16);
    {
      register struct ResultMenu *write asm("r1") = p;
      register int one asm("r0");
      asm("" : "+r"(write));
      write->state = zero;
      one = 1;
      asm("" : "+r"(one));
      write->entered = one;
      write->timer = zero;
    }
    {
      register void **callback asm("r0") = &p->onClose;
      asm("" : "+r"(callback));
      *callback = (void *)zero;
    }
  }
  return 0;
}
