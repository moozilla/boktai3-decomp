#include "global.h"
#include "scene_actor_reset.h"

// confidence=low: state advances segment interpolation and oscillation before restoring control.

// confidence=low: inferred from segment state and collider updates.

void TentacleBody_UpdateChargeSpread(void);
void TentacleActor_SelectWanderRoute(void *);
void sub_080A9DB0(void *, s32, s32);
void TentacleActor_UpdateOscillation(void *, s32, s32);
static inline u8 TakeEntry(struct ChaseActor *p)
{
    if (p->base.field2B1) {
        p->base.field2B1 = 0;
        return 1;
    }
    return 0;
}

static inline void SetControl(struct ChaseActor *s, void *control, u32 code)
{
    u32 zero;
    u32 *flags;
    u8 *field = &s->base.field2B2;
    zero = 0;
    *field = 1;
    s->base.stepDone = zero;
    s->base.controlKind = code;
    s->base.target = control;
    s->base.field2C8 = zero;
    flags = &s->base.pendingControlFlags;
    {
        u32 mask = -2;
        *flags &= mask;
    }
    s->base.pendingControl = zero;
}
static inline void SetState(struct ChaseActor *p, u32 b)
{
    u32 behavior = b;
    p->base.state2A8 = 0;
    p->base.phase2A9 = 0;
    p->base.behavior2AA = behavior;
    p->base.counter2AC = 0;
    p->base.field2B1 = 1;
}
static inline s32 Abs(s32 x)
{
    s32 r = x;
    if (x < 0)
        r = -x;
    return r;
}
static inline void Advance(void *p, s32 x, s32 z)
{

    u16 k[] = {0x6400};
    sub_080A9DB0(p, 4, k[0]);
}
union RetractionActorHandle {
    struct ChaseActor *ptr;
    u8 parts[4];
};
void TentacleActor_UpdateRetraction(struct ChaseActor *input)
{
    union RetractionActorHandle actor;
    u32 raw;
    actor.ptr = input;
    if (TakeEntry(actor.ptr)) {
        SetControl(actor.ptr, TentacleBody_UpdateChargeSpread, 0x23);
        actor.ptr->base.field2E8 = 8;
    }
    {
        s32 x = actor.ptr->base.secondaryMotion.x;
        s32 ax = x;
        s32 z;
        s32 az;
        if (x < 0)
            ax = -x;
        z = actor.ptr->base.secondaryMotion.z;
        az = z;
        if (z < 0)
            az = -z;
        if (ax > az) {
            if (x < 0) {
                Advance(actor.ptr, ax, az);
                raw = actor.parts[0];
                if ((s32)raw < 0)
                    actor.parts[0] = 0;
                raw = actor.parts[0];
            } else {
                Advance(actor.ptr, ax, az);
                raw = actor.parts[0];
                if ((s32)raw < 0)
                    actor.parts[0] = 0;
                raw = actor.parts[0];
            }
        } else {
            if (z < 0) {
                Advance(actor.ptr, ax, az);
                raw = actor.parts[0];
                if ((s32)raw < 0)
                    actor.parts[0] = 0;
                raw = actor.parts[0];
            } else {
                Advance(actor.ptr, ax, az);
                raw = actor.parts[0];
                if ((s32)raw < 0)
                    actor.parts[0] = 0;
                raw = actor.parts[0];
            }
        }
    }
    TentacleActor_UpdateOscillation(actor.ptr, 0x1ae, 0x28);
    if (actor.ptr->base.stepDone) {
        TentacleActor_SelectWanderRoute(actor.ptr);
        SetControl(actor.ptr, actor.ptr->base.onAlternateEntry, 1);
        SetState(actor.ptr, 2);
    } else {
        actor.ptr->base.counter2AC++;
    }
}
