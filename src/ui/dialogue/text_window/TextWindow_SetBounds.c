#include "global.h"

struct Win {
    u8 pad[4];
    u8 x, y, w, h;
};

s32 TextState_SetBounds(void *state, u32 left, u32 top, u32 width, u32 height);
void TextWindow_BlankRect(s32 x, s32 y, s32 width, s32 height);
void *TextWindow_Find(u32 id);

s32 TextWindow_SetBounds(u32 id, u32 x, u32 y, u32 w, u32 h)
{
    struct Win *win = TextWindow_Find(id);
    u8 *state;
    s32 r;

    if (win == 0) {
        r = -1;
    } else {
        state = (u8 *)win + 0x60;
        TextWindow_BlankRect(win->x, win->y, win->w, win->h);
        win->x = x;
        win->y = y;
        win->w = w;
        win->h = h;
        r = TextState_SetBounds(state, win->x, win->y, win->w, win->h);
    }
    return r;
}
