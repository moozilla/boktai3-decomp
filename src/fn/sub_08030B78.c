#include "global.h"
#include "ram.h"
struct State {
    u8 pad0[15];
    u8 fresh;
    u8 pad10[5];
    s8 current;
    u8 pad16[6];
    void *pc;
};
struct Render {
    u32 flags;
    u8 pad[12];
    u16 tile;
};
struct Frames {
    u16 *data;
    u8 flags, count, multiplier, duration;
    u16 index, pada, rate, timer;
};
struct Actor {
    u8 pad0[0x18];
    struct State state;
    u8 pad38[0x144];
    u8 x, y, width, height;
    u16 scriptIndex;
    u8 mode, pad183, previousMode, pad185[2], defaultMode;
    u8 pad188[14];
    u16 wait;
    u8 pad198[4];
    u32 counter;
    u8 pad1a0[4];
    void *script;
    u16 scriptFrame, scriptCount;
    u8 pad1ac[0x6c];
    u32 flags;
    u8 pad21c[0x54];
    u8 sprite[0x1c];
    struct Render render;
    u8 pad2a0[0x18];
    struct Frames frames;
    void *resource;
    void *callback;
};
int MessageWindow_UpdateSpeakerPortrait(struct Actor *);
void sub_080316A4(void);
void MessageWindow_UpdateImmediatePlayback(void);
void MessageWindow_ClearTextCompletion(void);
int TextWindow_BlankRect(int, int, int, int);
void *MessageWindow_GetStringAtOffset(struct Actor *, int);
void TextState_Reset(struct State *);
int TextState_SetBounds(struct State *, int, int, int, int);
int Script_GetValue(void);
static inline void setCallback(void *fn, struct Actor *p)
{
    p->callback = fn;
}
static inline void setCallbackB(struct Actor *p, void *fn)
{
    p->callback = fn;
}
int MessageWindow_SetDisplayMode(int mode)
{
    struct Actor *p = gMessageWindow;
    if (!p)
        return 0;
    p->mode = mode;
    if (!(u8)mode) {
        setCallback(sub_080316A4, p);
        {
            u8 *mode = &p->defaultMode;
            *mode = 3;
        }
    } else {
        p->counter = 0;
        setCallbackB(p, MessageWindow_UpdateImmediatePlayback);
    }
    {
        u8 *fresh = &p->state.fresh;
        *fresh = 0;
        fresh -= 15;
        ((struct State *)fresh)->current = -1;
    }
    return MessageWindow_UpdateSpeakerPortrait(p);
}
