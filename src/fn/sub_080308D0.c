#include "global.h"
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
    u8 pad188[12];
    u16 palette, wait;
    u8 pad198[4];
    u32 counter;
    u8 pad1a0[4];
    void *script;
    u16 scriptFrame, scriptCount;
    u8 pad1ac[0x40];
    void *paletteData;
    u8 pad1f0[0x28];
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
int sub_08218E5C(int, int, int, int);
void *sub_080313C8(struct Actor *, int);
void sub_08032C34(struct State *);
int sub_08032C54(struct State *, int, int, int, int);
int Script_GetValue(void);
static inline void setCallback(void *fn, struct Actor *p)
{
    p->callback = fn;
}
static inline void setCallbackB(struct Actor *p, void *fn)
{
    p->callback = fn;
}
void *sub_0821A520(int, u16);
int sub_080308D0(struct Actor *p, int palette)
{
    void *data = sub_0821A520(0x92b3, (u16)palette);
    int result;
    if (data) {
        p->palette = palette;
        p->paletteData = data;
        result = 0;
    } else
        result = -1;
    return result;
}
