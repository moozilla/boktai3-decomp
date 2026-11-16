#include "golem.h"
#include "golem_2.h"
#include "engine.h"

extern const u8 gUnk_08606134[];
void Camera_RequestShake(u32);
void Skeleton_TryTriggerAction(void *);
void Skeleton_ClearPendingFlag(void *);
void Golem_UpdateAttackProperties(void *);

static inline u32 Flags16(struct Golem *a, u32 m)
{
    u32 off = 0x2D4;
    return *(u16 *)((u8 *)a + off) & m;
}

static inline u32 Flags32(struct Golem *a, u32 m)
{
    u32 off = 0x2D0;
    return *(u32 *)((u8 *)a + off) & m;
}

static inline u8 Unavailable(struct Golem *a)
{
    if (a) {
        if (Flags16(a, 0x40) || Flags32(a, 0x20000) || a->chase.base.health <= 0)
            return 1;
    }
    return 0;
}

void Golem_InitActionHandlers(struct Golem *actor)
{
    actor->chase.base.alternateBehaviorCallbacks = (void **)gUnk_08606134;
}

s32 Golem_TickFacingQuadrant(struct Golem *a)
{
    u8 *base = (u8 *)&a->chase.base.body.id;
    struct GolemState *s = a->chase.base.context;
    if (!Unavailable(a)) {
        if (s->countdown626)
            s->countdown626--;
    }
    {
        u32 quadrant = (base[9] + 0x60) >> 6;
        u32 mask = 3;
        mask &= quadrant;
        a->field68C = mask;
    }
    a->savedDirection690 = a->direction68E;
    return 1;
}

s32 Golem_UpdateEffectsAndContacts(struct Golem *actor)
{
    struct GolemState *state = actor->chase.base.context;
    u32 m = 0x40;
    if (!(actor->chase.base.flags2D4 & m)) {
        u16 *p = &state->shakeTimer6AE;
        if (*p != 0) {
            Camera_RequestShake(state->shakeStrength6AC);
            *p = *p - 1;
        }
    }
    Skeleton_TryTriggerAction(actor);
    Skeleton_ClearPendingFlag(actor);
    Golem_UpdateAttackProperties(actor);
    return 1;
}
