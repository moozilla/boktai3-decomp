#include "global.h"

struct TextState {
    u8 pad0[0x1c];
    void *pc;
};

struct TextWindow {
    u8 pad0[0x10];
    u16 wait;
    u8 pad12[2];
    void *script;
    u16 frame, count;
    u16 frames[0x20];
    u8 pad5c[4];
    struct TextState text;
};
struct TextWindow *TextWindow_Find(s32);
void *TextWindow_ResolveString(struct TextWindow *, int);
void TextState_Reset(void *);

s32 sub_080338A8(s32 id, void *script, u32 count, u32 frame, u16 *frames)
{
    struct TextWindow *w = TextWindow_Find(id);
    struct TextState *state;
    s32 i;
    if (w == 0)
        return -1;
    state = &w->text;
    w->script = script;
    state->pc = TextWindow_ResolveString(w, 0);
    w->frame = frame;
    w->count = count;
    if ((u16)count > 0x20)
        w->count = 0x20;
    for (i = 0; i < w->count; i++)
        w->frames[i] = frames[i];
    w->wait = w->frames[w->frame];
    TextState_Reset(&w->text);
    return 0;
}
