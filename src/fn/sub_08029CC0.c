#include "global.h"
struct Common {
    u8 pad0[0x90];
    u32 timer;
    s16 health, maxHealth;
    u8 pad98[4];
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
struct Packed {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
void DamageNumberDisplay_Show(void *, int, int, void *, void *, int);
void sub_0802A4F4(void *);
void sub_0802A53C(void *);
void sub_0802A63C(void *);
static inline void action(struct Common *p, int id, int zero)
{
    int one = 1;
    p->active = one;
    p->actionReset = zero;
    p->action = id;
    p->actionTimer = zero;
    p->entered = one;
}
int sub_08029CC0(struct Common *p, int kind, int damage)
{
    u8 *position = (u8 *)p + 0x48;
    struct Packed size;
    switch (kind) {
        case 0:
            if (damage > 0) {
                size.x = 32;
                size.y = 0;
                size.z = 32;
                DamageNumberDisplay_Show((u8 *)p + 0x818, damage, 0, (u8 *)p + 0x54, &size, 1);
            }
            action(p, 0, 0);
            state(p, sub_0802A4F4, 5);
            break;
        case 2:
            action(p, 1, 0);
            state(p, sub_0802A53C, 6);
            {
                int angle = position[9];
                *((u8 *)p + 0x17e) = angle;
            }
            break;
        case 1: {
            int angle = (*(u8 **)((u8 *)p + 0x10c))[0x2d];
            u8 *dst = (u8 *)p + 0x17e;
            int zero = 0;
            *dst = angle;
            {
                u32 mask = 2;
                p->flags |= mask;
            }
            action(p, 3, zero);
            state(p, sub_0802A63C, 8);
        } break;
    }
    return 1;
}
