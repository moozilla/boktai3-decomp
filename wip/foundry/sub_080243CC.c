#include "global.h"
typedef struct State {
    u8 pad0[0x90];
    u32 tick;
    s16 hp, maxHp;
    u16 extra;
    u8 vulnerable, kind;
    u32 flags;
    u8 unknownA0[2];
    u8 unknownA2, unknownA3;
    u32 unknownA4;
    u8 facing, facing8, animation, state, mode, submode, phase, finished, enter, changed;
    u8 padB2[0x12];
    u32 elapsed;
    u8 padC8[0x38];
    void (*callback)(void *, int);
} State;
typedef struct Actor {
    State base;
    u8 pad104[0x27f];
    u8 variant;
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
void sub_080243CC(State *p)
{
    p->vulnerable = 1;
    enter(p);
    if (p->finished) {
        u8 v = ((Actor *)p)->variant;
        if (v) {
            u32 next = 8;
            p->phase = next;
            p->elapsed = 0;
        } else {
            u32 next = 7;
            p->phase = next;
            p->elapsed = v;
        }
        p->enter = 1;
    } else
        p->elapsed++;
}
