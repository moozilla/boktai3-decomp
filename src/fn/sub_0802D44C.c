#include "global.h"
struct Vec {
    s16 x, y, z, pad;
};
struct Object {
    u8 pad0[0xc];
    struct Vec pos;
    u8 pad14[0x3c];
    u32 flags;
    u8 pad54[0xc8];
    void *actionFn;
    void *selectFn;
    void *damageFn;
    u16 state, action;
    u8 stateChanged, actionChanged;
    u16 requested;
    u32 ticks, actionTicks;
    u8 pad138[0x4c];
    u32 drawable;
};
void Actor_InitControllerFromScript(struct Object *);
void DisplayAnimation_InitializeSequence(void *, int, int, int, struct Vec *);
int Actor_SetFacingAnimationEntry(struct Object *, int, int, int, int, int);
void Actor_InitHitSensor(struct Object *, struct Vec *, struct Vec *, void *);
void Actor_InitBodyCollider(struct Object *, struct Vec *, struct Vec *, void *);
void Actor_OnColliderContact(void);
void Actor_OnHitboxContact(void);
void sub_0802D334(void);
void Actor_OnDamage(void);
void Actor_OnTrackingAction(void);
static inline void selectCallback(struct Object *p, void *fn)
{
    p->selectFn = fn;
}
static inline void damageCallback(struct Object *p, void *fn)
{
    p->damageFn = fn;
}
static inline void state(struct Object *p, int zero)
{
    p->state = zero;
    p->stateChanged = 1;
    p->ticks = zero;
}
static inline void action(struct Object *p, void *fn, int zero, int one)
{
    p->actionFn = fn;
    p->action = zero;
    p->actionChanged = one;
    p->actionTicks = zero;
}
static inline void collider(struct Object *p, struct Vec *size, void *fn, struct Vec *offset)
{
    Actor_InitHitSensor(p, size, offset, fn);
}
static inline void hitbox(struct Object *p, struct Vec *size, void *fn, struct Vec *offset)
{
    Actor_InitBodyCollider(p, size, offset, fn);
}
// Scene actor initializer. Matches without register hints: the draft's pins
// and barriers were what made it differ.
int Actor_InitializeControllerFromScript(struct Object *p)
{
    struct Vec size, offset, size2, offset2;
    int zero;
    Actor_InitControllerFromScript(p);
    DisplayAnimation_InitializeSequence(&p->drawable, 0xd495, 0xaf44, 0, &p->pos);
    {
        int initialZero = 0;
        zero = initialZero;
        Actor_SetFacingAnimationEntry(p, 1, initialZero, 1, initialZero, initialZero);
    }
    {
        u32 mask = 2;
        p->flags |= mask;
    }
    size.x = 32;
    size.y = 128;
    size.z = 32;
    offset.x = 0;
    offset.y = 128;
    offset.z = 0;
    collider(p, &size, Actor_OnColliderContact, &offset);
    size2.x = 32;
    size2.y = 128;
    size2.z = 32;
    offset2.x = 0;
    offset2.y = 128;
    offset2.z = 0;
    hitbox(p, &size2, Actor_OnHitboxContact, &offset2);
    selectCallback(p, sub_0802D334);
    damageCallback(p, Actor_OnDamage);
    p->state = zero;
    p->stateChanged = 1;
    p->ticks = zero;
    action(p, Actor_OnTrackingAction, zero, 1);
    return 0;
}
