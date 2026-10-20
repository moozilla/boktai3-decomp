#include "global.h"
struct PackedVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
struct Sprite {
    u8 pad[9];
    u8 direction;
};
struct Ratatosk {
    u8 pad0[0x48];
    struct Sprite sprite;
    u8 pad54[0x3c];
    u32 timer;
    u8 pad94[0x8];
    u32 flags;
    u8 padA0[2];
    u8 mode;
    u8 padA3;
    u32 modeTime;
    u8 padA8[3];
    u8 animation, state, nextState, changed, padAF, active, reset;
    u8 padB2[0x12];
    u32 ticks;
    u8 padC8[0x38];
    void (*callback)(void);
    u8 pad104[8];
    u8 *player;
};
void DamageNumberDisplay_Show(void *, s32, s32, void *, void *, s32);
void Ratatosk_BeginDirectionalReaction(void);
void Ratatosk_UpdateDirectionalRecovery(void);
void sub_081F4CF8(void);
static inline void setState(struct Ratatosk *p, u32 state, u32 next, u32 changed)
{
    p->state = state;
    p->nextState = next;
    p->changed = changed;
    p->ticks = 0;
    p->active = 1;
}
static inline void setCallback(struct Ratatosk *p, void (*callback)(void), u32 animation)
{
    p->animation = animation;
    p->callback = callback;
    p->timer = 0;
    p->reset = 1;
    p->mode = 0;
    p->modeTime = 0;
}
s32 Ratatosk_ApplyDamageReaction(struct Ratatosk *p, s32 kind, s32 damage)
{
    struct Sprite *sprite = &p->sprite;
    struct PackedVec size;
    switch (kind) {
        case 0:
            if (damage > 0) {
                size.x = 32;
                size.y = 0;
                size.z = 32;
                DamageNumberDisplay_Show((u8 *)p + 0x188, damage, 0, (u8 *)p + 0x54, &size, 1);
            }
            setState(p, 1, 0, 2);
            setCallback(p, Ratatosk_BeginDirectionalReaction, 16);
            break;
        case 2:
            setState(p, 1, 0, 3);
            setCallback(p, Ratatosk_UpdateDirectionalRecovery, 17);
            {
                u32 direction = sprite->direction;
                *(u8 *)((u8 *)p + 0xe72) = direction;
            }
            break;
        case 1: {
            u32 direction = p->player[0x2d];
            u8 *dest = (u8 *)p + 0xe72;
            u32 zero = 0;
            *dest = direction;
            {
                u32 flags = 2;
                p->flags |= flags;
            }
            setState(p, 1, zero, 1);
            setCallback(p, sub_081F4CF8, 15);
        } break;
    }
    return 1;
}
