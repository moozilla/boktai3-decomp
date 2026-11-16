#include "global.h"
#include "scene_actor_chase.h"
#include "scene_actor_reset.h"
#include "scene_actor_animation.h"
#include "scene_actor_control_phase.h"
#include "scene_actor_reaction.h"
#include "engine.h"
#include "mummy.h"

// confidence=low: follows a navigation descriptor, switching between acceleration and turning.

s32 NavMesh_ConsumeProbeEvent(void *);

u8 SceneActorBehavior_AccelerateTowardDescriptor(void *, void *, s32, s32, s32);

u8 SceneActorBehavior_RequestDescriptorTurn(void *, void *, s32, s32, s32);

static inline u32 TestFlags(struct ChaseActor *a, u32 mask)
{
    return a->base.motionFlags & mask;
}

void Mummy_UpdateDescriptorPursuit(struct ChaseActor *a)
{
    u8 *group = (u8 *)&a->group.common;
    s32 motion;
    s32 event;
    if (a->base.field2B1)
        a->base.field2B1 = 0;
    if ((u8)Callback_InvokeActor(a, a->base.contactQuery.opaque))
        return;
    motion = Callback_InvokeActor(a, *(void **)((u8 *)a + 0x260));
    {
        u16 *rate;
        if (TestFlags(a, 0xA02F)) {
            u16 *p = &a->base.field2C2;
            *p = 64;
            rate = p;
        } else {
            u16 *p = &a->base.field2C2;
            u16 value = *p;
            rate = p;
            if (!value)
                *rate = 64;
        }
        if (!a->base.drawableKind)
            Anim_SetDurationScale((u8 *)a->base.drawable + 0x48, *rate);
        else
            Anim_SetFrameDurationScale((u8 *)a->base.drawable + 0x20, *rate);
    }
    event = NavMesh_ConsumeProbeEvent(*(void **)(group + 0x1FC));
    if (!event) {
        if (SceneActorBehavior_AccelerateTowardDescriptor(a, *(void **)(group + 0x1FC), 32, 32,
                                                          motion)) {
            u32 state = 0x12;
            a->base.state2A8 = event;
            a->base.phase2A9 = event;
            a->base.behavior2AA = state;
            a->base.counter2AC = event;
            a->base.field2B1 = 1;
            return;
        }
    } else {
        if (SceneActorBehavior_RequestDescriptorTurn(a, *(void **)(group + 0x1FC), 32, 32, motion))
            Chase_SetBehavior(a, 0x11);
    }
    a->base.counter2AC++;
}

s32 Callback_InvokeActor(void *, void *);

void Mummy_UpdatePlayerRoute(struct ChaseActor *actor);

static inline u8 TakePrepared(u8 *field)
{
    if (*field) {
        *field = 0;
        return 1;
    }
    return 0;
}

static inline void SetControl(struct ChaseActor *actor, void *control, u32 kind)
{
    u8 *prepared = &actor->base.field2B2;
    u32 zero = 0;
    *prepared = 1;
    actor->base.stepDone = zero;
    actor->base.controlKind = kind;
    actor->base.target = control;
    actor->base.field2C8 = zero;
    {
        u32 *flags = &actor->base.pendingControlFlags;
        s32 mask = -2;
        *flags &= mask;
    }
    actor->base.pendingControl = zero;
}

void SceneActor_WaitForControlCompletion(struct ChaseActor *actor)
{
    u8 busy;
    if (TakePrepared(&actor->base.field2B1)) {
        void *control = actor->base.onEntry;
        SetControl(actor, control, 2);
    }
    busy = Callback_InvokeActor(actor, actor->base.contactQuery.opaque);
    if (!busy) {
        if (actor->base.stepDone) {
            SceneActorAnimation_SelectDirectionalEntry((void *)actor, 0);
            {
                u32 state = 0x10;
                actor->base.state2A8 = 0;
                actor->base.phase2A9 = 0;
                actor->base.behavior2AA = state;
                actor->base.counter2AC = 0;
                actor->base.field2B1 = 1;
            }
            SetControl(actor, Mummy_UpdatePlayerRoute, 0x27);
        } else
            actor->base.counter2AC++;
    }
}

/* SceneActorControl_UpdatePersistentPhase_080D622C: SceneActorControl_UpdatePersistentPhase variant: control Mummy_UpdatePlayerRoute kind 39, no shadow toggle, update callback via Callback_InvokeActor (provisional name) */

void SceneActorControl_UpdatePersistentPhase_080D622C(struct ChaseActor *actor)
{
    if (SceneActorControl_TakeFlag(&actor->base.field2B1)) {
        void *control = (void *)Mummy_UpdatePlayerRoute;
        u32 kind;
        kind = 39;
        actor->base.field2B2 = 1;
        actor->base.stepDone = 0;
        actor->base.controlKind = kind;
        actor->base.target = control;
        actor->base.field2C8 = 0;
        {
            u32 *flags = &actor->base.pendingControlFlags;
            u32 mask = ~1;
            *flags &= mask;
        }
        actor->base.pendingControl = 0;
    }
    if (!(u8)Callback_InvokeActor(actor, *(void **)((u8 *)actor + 0x270)))
        ++actor->base.counter2AC;
}

extern const u8 gUnk_0860616C[];

struct SSetControlPreset {
    u8 filler[644];
    const u8 *p;
};

void Mummy_SetControlPreset(struct SSetControlPreset *s)
{
    s->p = gUnk_0860616C;
}

// confidence=low: animation checkpoint emits aimed shots with rising speed and falling strength.

s32 Div(s32, s32);

void BouncingObject_Spawn(struct ContactPackedVector *, struct ContactPosition *,
                          struct ContactPosition *, u32, u32, u32, u32);

extern const s16 gUnk_086149C4[];

static inline s32 Scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}

static inline void Subtract(struct ContactPosition *out, struct ContactPosition *from,
                            struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}

static inline s32 Abs(s32 value)
{
    return value < 0 ? -value : value;
}

void Mummy_FireAcceleratingVolley(struct ChaseActor *actor)
{
    u8 *body = actor->base.context;
    struct ContactPackedVector source = *(struct ContactPackedVector *)&actor->base.body.position;
    struct ContactPosition delta;
    struct ContactPosition *velocity;
    s32 divisor, facing, difference;
    s32 heightDifference;
    s32 speed;
    s32 count, i;
    s32 direction;
    s32 strength;
    source.y += 200;
    heightDifference = source.y - actor->base.tracking.target->position.y;
    if (heightDifference <= 255)
        divisor = 15;
    else if (heightDifference <= 511)
        divisor = 20;
    else if (heightDifference <= 767)
        divisor = 23;
    else if (heightDifference <= 1023)
        divisor = 27;
    else
        divisor = 34;
    Subtract(&delta, &actor->base.body.position, &actor->base.tracking.target->position);
    {
        s32 x = delta.x, z;
        s32 ax = Abs(x), az;
        z = delta.z;
        az = Abs(z);
        if (ax > az)
            speed = Div(x, divisor);
        else
            speed = Div(z, divisor);
        speed = Abs(speed);
    }
    if (speed <= 10)
        speed = 10;
    if (speed > *(u16 *)(body + 0x6cc))
        speed = *(u16 *)(body + 0x6cc);
    switch (actor->base.paletteOffset) {
        case 0:
            count = 1;
            break;
        case 1:
            count = 2;
            break;
        default:
            count = 3;
            break;
    }
    facing = ((actor->base.body.direction + 32) & 255) >> 6 << 6;
    difference = actor->base.tracking.heading - facing;
    if (difference > 128)
        difference -= 256;
    else if (difference < -128)
        difference += 256;
    direction = difference;
    if (Abs(direction) > 28)
        direction = facing;
    else
        direction = actor->base.tracking.heading;
    i = count;
    strength = *(u16 *)(body + 0x6ce);
    for (; i != 0; i--) {
        velocity = &delta;
        velocity->x = Scale(gUnk_086149C4[(direction + 64) & 255] * speed);
        velocity->y = 0;
        velocity->z = Scale(gUnk_086149C4[direction & 255] * speed);
        {
            u32 shot = actor->base.attacks.first[2];
            u32 category = actor->base.attacks.second[2];
            void *target = &actor->base.tracking.target->position;
            BouncingObject_Spawn(&source, &delta, target, shot, category, strength,
                                 actor->base.body.id);
        }
        strength = Div(strength << 3, 10);
        speed += Div(speed << 2, 10);
    }
    Sound_PlaySong(0x101);
}

void SceneActorControl_OnIdle(u8 *);

void Mummy_EnterIdleControl(struct ChaseActor *actor)
{
    struct Mummy *context = actor->base.context;
    if (actor->base.tracking.distance < context->limit) {
        void *fn = SceneActorControl_OnIdle;
        u8 code = 0x10;
        u32 zero;
        u8 one;
        u8 zero8;
        u32 two;
        u8 *taken = &actor->base.field2B2;
        zero = 0;
        one = 1;
        *taken = one;
        actor->base.stepDone = zero;
        actor->base.controlKind = code;
        actor->base.target = fn;
        zero8 = 0;
        actor->base.field2C8 = zero;
        {
            u32 *flags = &actor->base.pendingControlFlags;
            u32 mask = -2;
            *flags &= mask;
        }
        actor->base.pendingControl = zero;
        two = 2;
        actor->base.state2A8 = zero8;
        actor->base.phase2A9 = two;
        actor->base.behavior2AA = zero8;
        actor->base.counter2AC = zero;
        actor->base.field2B1 = one;
    } else {
        void *fn = SceneActorControl_OnIdle;
        u8 code = 0x10;
        u32 zero;
        u8 one;
        u8 zero8;
        u32 two;
        u8 *taken = &actor->base.field2B2;
        zero = 0;
        one = 1;
        *taken = one;
        actor->base.stepDone = zero;
        actor->base.controlKind = code;
        actor->base.target = fn;
        zero8 = 0;
        actor->base.field2C8 = zero;
        {
            u32 *flags = &actor->base.pendingControlFlags;
            u32 mask = -2;
            *flags &= mask;
        }
        actor->base.pendingControl = zero;
        two = 2;
        actor->base.state2A8 = zero8;
        actor->base.phase2A9 = two;
        actor->base.behavior2AA = one;
        actor->base.counter2AC = zero;
        actor->base.field2B1 = one;
    }
}
