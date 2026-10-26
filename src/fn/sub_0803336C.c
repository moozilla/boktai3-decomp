#include "global.h"
#include "render.h"
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
int ScrollingDialogue_ResetPlayback(struct Window *, int);
int ScrollingDialogue_Init(struct Window *p)
{
    struct TextState *t;
    int zero;
    gUnk_020000E0 = p;
    t = &p->text;
    {
        u8 *active = &p->active;
        zero = 0;
        *active = zero;
    }
    p->showRequest = zero;
    p->hideRequest = zero;
    p->frames = zero;
    p->row = zero;
    p->column = zero;
    TextState_Init(t, 0, 18, 2, 2);
    t->pad14 = zero;
    p->script = gUnk_086056D0;
    t->pc = gUnk_086056D0;
    ScrollingDialogue_ResetPlayback(p, 0);
    t->voice = zero;
    t->skip = 1;
    p->ticks = zero;
    gUnk_03003A48 = zero;
    return 0;
}
