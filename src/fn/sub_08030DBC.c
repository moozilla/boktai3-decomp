#include "global.h"
#include "ram.h"
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
static inline u8 getMode(struct Actor *p, u32 offset)
{
    return *((u8 *)p + offset);
}
int MessageWindow_ShowMessage(int index)
{
    struct Actor *p = gMessageWindow;
    if (p) {
        void *script = p->script;
        struct State *state = &p->state;
        asm("" : "+r"(state) : "r"(script) : "r1");
        if (script) {
            TextWindow_BlankRect(p->x, p->y, p->width, p->height);
            TextState_SetBounds(state, p->x, p->y, p->width, p->height);
            p->scriptIndex = index;
            {
                void *pc = MessageWindow_GetStringAtOffset(p, p->scriptIndex);
                struct State *dst = state;
                dst->pc = pc;
            }
            {
                u8 mode;
                {
                    u32 offset = 0x182;
                    u8 *ptr;
                    ptr = (u8 *)p + offset;
                    mode = *ptr;
                }
                if (!mode) {
                    {
                        void *fn = sub_080316A4;
                        void **dst;
                        u32 offset = 0x2cc;
                        dst = (void **)((u8 *)p + offset);
                        *dst = fn;
                    }
                    {
                        u32 offset = 0x187;
                        u8 *mode;
                        mode = (u8 *)p + offset;
                        *mode = 3;
                    }
                } else {
                    {
                        u32 offset = 0x19c;
                        u32 *counter;
                        counter = (u32 *)((u8 *)p + offset);
                        {
                            u32 zero = 0;
                            *counter = zero;
                        }
                    }
                    {
                        void *fn = MessageWindow_UpdateImmediatePlayback;
                        u32 offset;
                        void **dst;
                        offset = 0x2cc;
                        dst = (void **)((u8 *)p + offset);
                        *dst = fn;
                    }
                }
            }
            TextState_Reset(state);
            {
                u32 offset = 0x196;
                u16 *wait;
                wait = (u16 *)((u8 *)p + offset);
                {
                    u32 value = 65535;
                    *wait = value;
                }
            }
            {
                u32 offset = 0x218;
                u32 *flags;
                flags = (u32 *)((u8 *)p + offset);
                {
                    u32 value = *flags;
                    u32 mask = 1;
                    value |= mask;
                    *flags = value;
                }
            }
        }
    }
    return 0;
}
