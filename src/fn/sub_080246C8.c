#include "global.h"
#include "contact_actor.h"
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
    u8 pad104[0x2A0];
    s32 f3a4;
    u8 pad3a8[0x630];
    void *pool;
} Actor;

void ContactActorPool_Start();

static inline u8 enter(State *p)
{
    if (p->enter) {
        p->enter = 0;
        p->finished = 0;
        return 1;
    }
    return 0;
}

void SceneActorState_StartContactPool(Actor *a)
{
    State *p = &a->base;
    struct ContactPosition pos;
    struct ContactPosition pos2;
    u8 *src = (u8 *)p + 0x48;

    if (enter(p)) {
        u32 k = 0xE;
        p->animation = k;
    }
    if (p->finished) {
        struct ContactPosition *pp = &pos;
        struct ContactPosition *pp2;
        pos = *(struct ContactPosition *)(src + 0xC);
        pp->y += 200;
        pp2 = &pos2;
        pos2 = *pp;
        {
            s32 v = p->hp;
            a->f3a4 = v;
        }
        if (a->pool)
            ContactActorPool_Start(a->pool, pp, pp2, 10, 10);
        {
            u32 next = 0xC;
            p->phase = next;
        }
        p->elapsed = 0;
        p->enter = 1;
    } else
        p->elapsed++;
}
