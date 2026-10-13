#include "global.h"
// MATCHING_CONCESSION: register-pinning
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
extern struct Actor *gUnk_020000DC;
int sub_0803092C(struct Actor *);
void sub_080316A4(void);
void sub_080317D8(void);
void sub_080317CC(void);
int TextWindow_BlankRect(int, int, int, int);
void *sub_080313C8(struct Actor *, int);
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
int sub_08030BF8(void)
{
    struct Actor *p = gUnk_020000DC;
    register int result asm("r0");
    if (p)
        goto run;
    result = 0;
    goto done;
run:
    result = TextWindow_BlankRect(p->x, p->y, p->width, p->height);
    {
        register void *fn asm("r2") = sub_080317CC;
        void **dst;
        dst = &p->callback;
        *dst = fn;
    }
    p->flags |= 1;
    {
        register struct State *state asm("r2") = &p->state;
        state->current = -1;
    }
    p->render.flags |= 1;
    p->wait = 65535;
done:
    return result;
}
