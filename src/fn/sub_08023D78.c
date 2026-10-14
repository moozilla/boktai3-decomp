#include "global.h"
struct Common {
    u8 pad0[0x90];
    u32 timer;
    s16 health, maxHealth;
    u8 pad98[2];
    u8 healthCheck, pad9b;
    u32 flags;
    u8 padA0[2];
    u8 reset;
    u8 padA3;
    u32 pending;
    u8 facing, facing8, requested, state;
    u8 active, actionReset, action, finished, entered, changed;
    u8 padB2[0x12];
    u32 actionTimer;
    u8 padC8[0x38];
    void (*callback)(void *);
};
static inline void state(struct Common *p, void (*fn)(void *), int id)
{
    int zero;
    p->state = id;
    zero = 0;
    p->callback = fn;
    p->timer = zero;
    p->changed = 1;
    p->reset = zero;
    p->pending = zero;
}
int sub_0802AD0C(void *);
void sub_08027214(void *);
static inline u8 chk(struct Common *p)
{
    if (p->entered) {
        p->entered = 0;
        p->finished = 0;
        return 1;
    }
    return 0;
}
static inline void action(struct Common *p, int one, int id, int zero)
{
    p->active = one;
    p->actionReset = zero;
    p->action = id;
    p->actionTimer = zero;
    p->entered = one;
}
int sub_08023D78(struct Common *p)
{
    if (p->health <= 0 && p->healthCheck) {
        u32 mask = 2;
        p->flags |= mask;
        state(p, sub_08027214, 25);
    }
    p->healthCheck = 0;
    {
        u32 mask = 2;
        if (p->flags & mask) {
            action(p, 1, 3, 0);
            return 1;
        } else if (p->flags & 1) {
            action(p, 1, 2, 0);
            return 1;
        }
    }
    return 0;
}
