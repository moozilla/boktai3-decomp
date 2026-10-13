#include "global.h"
struct Sprite {
    u8 pad0[7];
    u8 visible;
    u8 pad8[9];
    u8 priority;
    u8 pad12[14];
};
struct Drawing {
    u8 pad0[0x1a];
    u8 changed;
    u8 pad1b[0x45];
};
struct Item {
    u8 state, entered, unused, count;
    u8 pad4[4];
    u16 timer;
    u8 pada[0x1a];
    struct Sprite sprite;
    u8 pad44[12];
    struct Drawing drawing;
};
extern const u16 gUnk_0824DE7C[];
extern const u8 gUnk_0824DE94[];
void Sprite_Init(void *, void *, int);
void sub_0821983C(void *, void *, int, int, int, int, int, const void *);
void sub_0803A2A8(void *owner, struct Item *p, int index)
{
    int state;
    int choice;
    int zero;
    struct Sprite *sprite;
    if ((u32)index <= 3 || (u32)(index - 8) <= 3)
        state = 1;
    else
        state = 0;
    zero = 0;
    p->count = zero;
    p->unused = zero;
    sprite = &p->sprite;
    Sprite_Init(sprite, (u8 *)owner + 0x20, 0x10011);
    {
        int clear = 0;
        u8 *visible;
        *(u16 *)((u8 *)sprite + 0x10) = zero;
        visible = (u8 *)p + 0x2b;
        *visible = clear;
    }
    if (index > 7)
        choice = (index & 3) + 4;
    else
        choice = index & 3;
    {
        int animation = gUnk_0824DE7C[choice];
        sub_0821983C(&p->drawing, (u8 *)owner + 0x3c, animation, 17, 0, 0, 60, gUnk_0824DE94);
    }
    p->drawing.changed = 1;
    p->state = state;
    p->entered = 1;
    p->timer = 0;
}
