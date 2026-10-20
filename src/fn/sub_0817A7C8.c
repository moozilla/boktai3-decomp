#include "global.h"
struct V {
    u8 pad0[0x51];
    u8 angle;
    u8 pad52[0x90 - 0x52];
    u32 timer;
    s16 health;
    u8 pad96[0x9c - 0x96];
    u32 flags;
    u8 padA0[2];
    u8 stateReset;
    u8 padA3;
    u32 pending;
    u8 padA8[2];
    u8 requested;
    u8 state;
    u8 active;
    u8 actionReset;
    u8 action;
    u8 finished;
    u8 entered;
    u8 changed;
    u8 padB2[0xC4 - 0xB2];
    u32 actionTimer;
    u8 padC8[0x100 - 0xC8];
    void (*callback)(void *);
    u8 pad104[0x10C - 0x104];
    u8 *damageSource;
    u8 pad110[0x46A - 0x110];
    s16 lastDamage;
    u8 pad46C[0x471 - 0x46C];
    u8 damageAngle;
};
void SceneVolume_BeginPhase(void *, int);
void sub_0817E378(void *);
static inline void StartAction(struct V *p, int id, int zero)
{
    int one = 1;
    p->active = one;
    p->actionReset = zero;
    p->action = id;
    p->actionTimer = zero;
    p->entered = one;
}
int sub_0817A7C8(struct V *p, int kind, int damage)
{
    u8 *position = (u8 *)p + 0x48;
    int zero0;
    void (*cb)(void *);
    u32 k2;
    switch (kind) {
        case 0: {
            int zero;
            int state;
            p->health += damage;
            p->lastDamage = damage;
            SceneVolume_BeginPhase(p, 2);
            zero0 = 0;
            StartAction(p, zero0, 0);
            cb = sub_0817E378;
            state = 0xb;
            p->state = state;
            p->callback = cb;
            p->timer = kind;
            p->changed = 1;
            p->stateReset = 0;
            p->pending = kind;
            p->requested = state;
            break;
        }
        case 2:
            StartAction(p, 1, 0);
            {
                int rq = 0xc;
                p->requested = rq;
            }
            {
                int angle = position[9] + 0x80;
                p->damageAngle = angle;
            }
            break;
        case 1: {
            int angle = p->damageSource[0x2d];
            u8 *dst = &p->damageAngle;
            int zero = 0;
            *dst = angle;
            {
                u32 mask = 2;
                p->flags |= mask;
            }
            StartAction(p, 3, zero);
            k2 = 0xd;
            p->requested = k2;
        } break;
    }
    return 1;
}
