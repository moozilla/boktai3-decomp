#include "global.h"
struct TextState {
    u8 x, y, left, top, width, height, palette, style;
    u8 entered, mode, skip, busy, padC, choiceCount, padE, fresh, pad10, voice, toggle, lastWidth,
        pad14;
    s8 current;
    u8 instant, wrap;
    u32 frame;
    const u8 *pc, *streams[4];
    u8 pad30[0xD8];
    u8 depth, styles[7];
};
// MATCHING_CONCESSION: register-pinning
const u8 *sub_0803232C(struct TextState *, const u8 *);
u16 sub_08032070(const u8 *);
int sub_08031F58(struct TextState *, u32);
int sub_08031FA0(struct TextState *, u32);
int sub_08031ED8(struct TextState *, int);
void sub_08031EAC(struct TextState *);
u32 sub_08218C5C(void);
const u8 *sub_08032A44(struct TextState *p, const u8 *s)
{
    u32 c;
    u16 glyph;
    goto test;
tag:
    s = sub_0803232C(p, s);
    if (p->padE)
        goto done;
    if (*s)
        goto next;
    goto done;
plain:
    asm("" ::: "memory");
    c = *s;
    if (c == 10) {
        sub_08031EAC(p);
        goto next;
    }
    if (c == 31) {
        u32 code = (u16)sub_08032070(s);
        if (code == 0xFFFF) {
            glyph = *s - 32;
            sub_08031F58(p, glyph);
        } else if (code & 0xFF00) {
            u32 mask = 0x7FFF;
            code &= mask;
            sub_08031FA0(p, code);
        } else
            sub_08031F58(p, code);
        s += 2;
        goto done;
    }
    asm("" ::: "memory");
    glyph = *s - 32;
    sub_08031F58(p, glyph);
    s++;
    goto done;
wide: {
    register int low asm("r1");
    asm("" ::: "memory");
    low = *s;
    {
        u32 mask = 0x7F;
        c = mask & low;
    }
    s++;
    low = *s;
    glyph = (c << 8) | low;
}
    if (glyph >= sub_08218C5C())
        sub_08031ED8(p, 1);
    else
        sub_08031FA0(p, glyph);
    s++;
    goto done;
next:
    s++;
test:
    c = *s;
    c <<= 24;
    if ((s32)c < 0)
        goto wide;
    c >>= 24;
    if (!c)
        goto done;
    if (c != '<')
        goto plain;
    s++;
    if (*s)
        goto tag;
done:
    return s;
}
