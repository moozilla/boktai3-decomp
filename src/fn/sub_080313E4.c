#include "global.h"
#include "input.h"
struct State {
    u8 pad0[7];
    u8 pending, entered, mode, skip;
    u8 padb[4];
    u8 fresh;
    u8 pad10[5];
    s8 current;
    u8 instant;
    u8 pad17[5];
    void *pc;
};
struct Actor {
    u8 pad0[0x18];
    struct State state;
    u8 pad38[0x144];
    u8 x, y, width, height;
    u16 scriptIndex;
    u8 mode, fast, previousMode, pad185, scrollMode, defaultMode;
    u16 scrollStep, scrollCounter;
    u32 waitFrames;
    u32 waitCounter;
    u16 palette, wait;
    u32 scrollFlags;
    u32 counter;
    u8 pad1a0[4];
    void *script;
    u16 scriptFrame, scriptCount;
    u8 pad1ac[0x44];
    u8 render[0x20];
    u8 frames[8];
    u32 flags;
    u8 pad21c[0xb0];
    void *callback;
};
struct Sun {
    u8 pad[0x12];
    s16 level;
};
extern struct Sun *gGameState;
void sub_080316A4(void);
void MessageWindow_UpdateFastPlayback(void);
int TextWindow_BlankRect(int, int, int, int);
void *MessageWindow_GetStringAtOffset(struct Actor *, int);
void TextState_Reset(struct State *);
int TextState_SetBounds(struct State *, int, int, int, int);
void Sound_PlaySong(u32);
s32 Anim_Step(void *, void *);

static inline void setCallback(void *fn, struct Actor *p)
{
    p->callback = fn;
}
static inline s32 GetLevel(void)
{
    return gGameState->level;
}

void sub_080313E4(struct Actor *p)
{
    struct State *st = &p->state;
    s32 result = 0;

    if (p->scrollMode) {
        if (p->scrollCounter++ >= p->scrollStep) {
            if (p->waitCounter >= p->waitFrames) {
                p->waitFrames = result;
                p->waitCounter = result;
                p->scrollCounter = result;
                result = 2;
            } else {
                p->waitCounter++;
            }
        }
    } else if (p->waitCounter >= p->waitFrames) {
        p->waitFrames = result;
        p->waitCounter = result;
        if (gPadState[0] & 2) {
            p->fast = 1;
            result = 1;
        }
        if (gPadState[1] & 1)
            result = 1;
        p->flags &= ~1;
    } else {
        p->fast = result;
        p->flags |= 1;
        p->waitCounter++;
    }
    if ((p->fast || result) && p->script) {
        p->scriptIndex++;
        if (p->fast) {
            setCallback(MessageWindow_UpdateFastPlayback, p);
            st->mode = 0;
            st->entered = 0;
        } else {
            setCallback(sub_080316A4, p);
            p->defaultMode = 3;
            st->mode = GetLevel();
        }
        st->pc = MessageWindow_GetStringAtOffset(p, p->scriptIndex);
        if (result != 2)
            Sound_PlaySong(0x107);
        TextWindow_BlankRect(p->x, p->y, p->width, p->height);
        TextState_SetBounds(st, p->x, p->y, p->width, p->height);
        TextState_Reset(&p->state);
        p->flags |= 1;
    } else {
        Anim_Step(p->frames, p->render);
    }
}
