#include "global.h"
#include "render.h"
#include "ram.h"
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
extern u32 gUnk_03003A48;
extern u16 gUnk_03005260[];
extern const u8 gUnk_086056D0[];
void TextWindow_BlankRect(int, int, int, int);
void BackgroundMap_ClearTextRect(int, int, int, int);
void TextState_SetBounds(struct TextState *, int, int, int, int);
void TextState_Init(struct TextState *, int, int, int, int);
void TextState_Reset(struct TextState *);
const u8 *TextState_GetActiveStream(struct TextState *);
int TextState_StepPlayback(struct TextState *);
int TextState_DrawNarrowGlyph(struct TextState *, int);
void TextState_ClearSelectedChoices(struct TextState *);
void TextState_RunSelectedProcedures(struct TextState *);
void ScrollingDialogue_UpdatePlayback(struct Window *);
int ScrollingDialogue_ClearScript(void);
void ScrollingDialogue_UpdatePlayback(struct Window *p)
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
            const u8 *q = TextState_GetActiveStream(t);
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
    TextWindow_BlankRect(p->row, 18, 1, 2);
    BackgroundMap_ClearTextRect(p->row, 18, 1, 2);
    p->row++;
    s = TextState_GetActiveStream(t);
    if (!s)
        goto wrap;
tags:
    if (*s != '<')
        goto render;
    TextState_StepPlayback(t);
    if (!t->padE)
        goto render;
    s = TextState_GetActiveStream(t);
    if (t->style)
        goto tags;
    if (*s)
        goto tags;
    p->wait = 8;
    goto tags;
render:
    if ((s8)*s >= 0) {
        TextState_SetBounds(&p->text, p->column, 18, 1, 2);
        p->column++;
        if (p->wait > 0) {
            TextState_DrawNarrowGlyph(t, 0);
            p->wait--;
            if (p->wait > 0)
                goto wrap;
            t->pc = p->script;
            p->wait = 0;
            goto wrap;
        }
        if (TextState_StepPlayback(t))
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
        TextState_DrawNarrowGlyph(t, 0);
        p->column++;
        p->wait--;
        if (p->wait > 0)
            goto wrap;
        t->pc = p->script;
        p->wait = 0;
    } else {
        TextState_SetBounds(&p->text, p->column, 18, 2, 2);
        p->column += 2;
        if (TextState_StepPlayback(t))
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
        ScrollingDialogue_UpdatePlayback(p);
}
