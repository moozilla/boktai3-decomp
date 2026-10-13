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
struct Window {
    u8 pad0[0x18];
    struct TextState text;
    u8 pad128[0x54];
    u8 active, showRequest, hideRequest, scrolling, delay, blocked, pad182[2];
    u32 frames, ticks;
    s32 column, row, wait;
    const u8 *script;
};
extern struct Window *gUnk_020000E0;
extern u32 gUnk_03003A48, gUnk_03004280;
extern u16 gUnk_03005260[];
extern const u8 gUnk_086056D0[];
void sub_08218E5C(int, int, int, int);
void sub_08218ED8(int, int, int, int);
void sub_08032C54(struct TextState *, int, int, int, int);
void sub_08032C80(struct TextState *, int, int, int, int);
void sub_08032C34(struct TextState *);
const u8 *sub_08032BEC(struct TextState *);
int sub_08032B24(struct TextState *);
int sub_08031F58(struct TextState *, int);
void sub_08032E8C(struct TextState *);
void sub_08032E98(struct TextState *);
void sub_08032F78(struct Window *);
int sub_08033468(void);
void sub_08032F78(struct Window *p)
{
    struct TextState *t = &p->text;
    int advance;
    const u8 *s;
    {
        struct TextState *initial = t;
        t = initial;
    }
    advance = 0;
    if (p->scrolling) {
        if (!p->delay) {
            if (gUnk_03005260[0] & 4)
                p->ticks = (p->ticks & ~3) + 4;
            else
                p->ticks++;
            advance = 1;
        } else
            p->delay--;
    } else {
        p->ticks = (p->ticks & ~7) + 8;
        advance = 1;
    }
    if (p->ticks & 7)
        goto done;
    if (p->scrolling && !p->delay && p->blocked)
        p->blocked = 0;
    if (!p->wait && !p->scrolling && p->column == 31 && (p->ticks & 15) == 8) {
        {
            const u8 *q = sub_08032BEC(t);
            if (q) {
                int c = (s8)*q;
                if (c < 0) {
                    p->ticks -= 8;
                    p->scrolling = 1;
                    p->blocked = 1;
                }
            }
        }
    }
    if (p->blocked)
        goto done;
    if (!advance)
        goto done;
    sub_08218E5C(p->row, 18, 1, 2);
    sub_08218ED8(p->row, 18, 1, 2);
    p->row++;
    s = sub_08032BEC(t);
    if (!s)
        goto wrap;
tags:
    if (*s != '<')
        goto render;
    sub_08032B24(t);
    if (!t->padE)
        goto render;
    s = sub_08032BEC(t);
    if (t->style)
        goto tags;
    if (*s)
        goto tags;
    p->wait = 8;
    goto tags;
render:
    if ((s8)*s >= 0) {
        sub_08032C54(&p->text, p->column, 18, 1, 2);
        p->column++;
        if (p->wait > 0) {
            sub_08031F58(t, 0);
            p->wait--;
            if (p->wait > 0)
                goto wrap;
            t->pc = p->script;
            p->wait = 0;
            goto wrap;
        }
        if (sub_08032B24(t))
            p->wait = 8;
        goto wrap;
    }
    if (!(p->column & 1)) {
        if ((p->ticks & 15) == 0)
            goto paintWide;
        goto wrap;
    }
    if ((p->ticks & 15) != 8)
        goto wrap;
paintWide:
    if (p->wait > 0) {
        sub_08031F58(t, 0);
        p->column++;
        p->wait--;
        if (p->wait > 0)
            goto wrap;
        t->pc = p->script;
        p->wait = 0;
    } else {
        sub_08032C54(&p->text, p->column, 18, 2, 2);
        p->column += 2;
        if (sub_08032B24(t))
            p->wait = 8;
    }
wrap:
    if (p->column > 31) {
        if (!p->scrolling)
            p->scrolling = 1;
        p->column -= 32;
    }
    if (p->row > 31)
        p->row -= 32;
done:
    if (!p->scrolling)
        sub_08032F78(p);
}
