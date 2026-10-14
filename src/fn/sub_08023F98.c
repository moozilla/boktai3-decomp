#include "global.h"
typedef struct State {
    u8 pad0[0x90];
    u32 tick;
    s16 hp;
    s16 maxHp;
    u16 extra;
    u8 vulnerable;
    u8 kind;
    u32 flags;
    u8 unknownA0[2];
    u8 unknownA2;
    u8 unknownA3;
    u32 unknownA4;
    u8 facing;
    u8 facing8;
    u8 animation;
    u8 state;
    u8 mode;
    u8 submode;
    u8 phase;
    u8 finished;
    u8 enter;
    u8 changed;
    u8 padB2[0x12];
    u32 elapsed;
    u8 padC8[0x38];
    void (*callback)(void *, int);
} State;
typedef struct Actor {
    State base;
    u8 pad104[0x23f];
    u8 variant;
    u8 pad344[0x12];
    u16 delay;
} Actor;
static inline u8 enter(State *p)
{
    if (p->enter) {
        p->enter = 0;
        p->finished = 0;
        return 1;
    }
    return 0;
}
void sub_08023F98(State *p)
{
    int bit;
    {
        u32 next = 6;
        u32 zero;
        p->phase = next;
        zero = 0;
        p->elapsed = zero;
        p->enter = 1;
    }
    {
        u32 k = 9;
        p->animation = k;
    }
    (*((void (**)(void *))(((u8 *)p) + 0x108)))(p);
    p->animation = 0;
    {
        u32 *flags = &p->flags;
        u32 mask = ~0x40u;
        bit = 0x40;
        p->flags &= ~bit;
    }
    ((u8 *)p)[0x383] = 0;
}
