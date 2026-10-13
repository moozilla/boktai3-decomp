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
void TextWindow_BlankRect(int, int, int, int);
void sub_08218ED8(int, int, int, int);
void TextState_SetBounds(struct TextState *, int, int, int, int);
void sub_08032C80(struct TextState *, int, int, int, int);
void TextState_Reset(struct TextState *);
const u8 *sub_08032BEC(struct TextState *);
int sub_08032B24(struct TextState *);
int sub_08031F58(struct TextState *, int);
void sub_08032E8C(struct TextState *);
void sub_08032E98(struct TextState *);
void sub_08032F78(struct Window *);
int sub_08033468(void);
void sub_08032EDC(struct Window *);
int sub_08032F14(struct Window *p, int clear)
{
    struct TextState *t = &p->text;
    int zero;
    s32 *row;
    if (clear)
        sub_08032EDC(p);
    row = &p->row;
    zero = 0;
    *row = zero;
    p->column = zero;
    p->ticks = zero;
    p->scrolling = zero;
    p->delay = 30;
    p->blocked = zero;
    p->wait = zero;
    t->pc = p->script;
    t->style = zero;
    t->wrap = 2;
    TextState_Reset(t);
}
