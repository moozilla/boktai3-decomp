// MATCHING_CONCESSION: register-pinning
#include "global.h"
// confidence=low: follows the route probe, then selects a new route point outside link mode.

#include "scene_actor_control_phase.h"
#include "engine.h"
#include "ram.h"
u8 SceneActor_IsFacingNearbyTarget(struct SceneActorRegistryResetView *);
s32 Callback_InvokeActor(void *, void *);
u8 sub_0812F3CC(struct SceneActorRegistryResetView *);
void SceneActorPhase_SelectLinkedAnimation(void);
void Anim_SetDurationScale(void *, u32);
void Anim_SetFrameDurationScale(void *, u32);
u32 NavMesh_ConsumeProbeEvent(void *);
u8 SceneActorBehavior_RequestDescriptorTurn(void *, void *, u32, u32, s32);
u8 SceneActorBehavior_AccelerateTowardDescriptor(void *, void *, u32, u32, s32);

#define FIELD(p, off, T) (*(T *)((u8 *)(p) + (off)))

static inline void SetState(struct SceneActorRegistryResetView *actor)
{
    u32 behavior = 6;
    register u32 offset asm("r4");
    u8 *field;
    actor->state2A8 = 0;
    offset = 0x2A9;
    field = (u8 *)actor + offset;
    *field = 0;
    actor->behavior2AA = behavior;
    offset += 3;
    field = (u8 *)actor + offset;
    *(u32 *)field = 0;
    actor->field2B1 = 1;
}
static inline void BeginRouteSelection(struct SceneActorRegistryResetView *actor, u8 *nav)
{
    register u32 behaviorOffset asm("r4");
    void *control;
    u32 stateOffset = 0x2A8;
    register u32 controlOffset asm("r1");
    u32 kind;
    u8 *field;
    u32 behavior = 8;
    actor->state2A8 = 0;
    actor->phase2A9 = 0;
    behaviorOffset = 0x2AA;
    field = (u8 *)actor + behaviorOffset;
    *field = behavior;
    actor->counter2AC = 0;
    stateOffset += 9;
    field = (u8 *)actor + stateOffset;
    *field = 1;
    /* The route callback is installed after the reset fields. */
    control = FIELD(nav, 0x214, void *);
    kind = 0x13;
    behaviorOffset += 8;
    field = (u8 *)actor + behaviorOffset;
    *field = 1;
    controlOffset = 0x2B0;
    FIELD(actor, controlOffset, u8) = 0;
    behaviorOffset += 3;
    field = (u8 *)actor + behaviorOffset;
    *field = kind;
    controlOffset -= 0x94;
    field = (u8 *)actor + controlOffset;
    *(void **)field = control;
    {
        register u32 timerOffset asm("r2") = 0x2C8;
        asm("" : : "r"(timerOffset));
        field = (u8 *)actor + timerOffset;
        *(u16 *)field = 0;
    }
    {
        u32 *flags = &actor->flags114;
        s32 mask = ~1;
        *flags &= mask;
    }
    behaviorOffset = 0x104;
    asm("" : : "r"(behaviorOffset));
    field = (u8 *)actor + behaviorOffset;
    *(u32 *)field = 0;
}

s32 Mod(s32, s32);
void SceneActorMovement_ResolveRoutePosition(void *);
struct RoutePoints {
    u8 count, unknown1;
    u16 offset;
};
struct RouteSelection {
    u8 unknown0[2];
    u8 index, phase;
    struct RoutePoints *points;
    void *current;
};
static inline u8 InLinkMode(u32 value, u32 mask)
{
    if (value & mask)
        return 1;
    return 0;
}
void SceneActorControl_UpdateRouteApproach(struct SceneActorRegistryResetView *actor)
{
    u8 *nav = (u8 *)actor + 0x3D4;
    s32 speed;
    u32 event;
    u16 *scale;
    if (actor->field2B1)
        actor->field2B1 = 0;
    speed = Callback_InvokeActor(actor, FIELD(actor, 0x260, void *));
    {
        u32 mask = 0xA02F;
        register u32 flagOffset asm("r4") = 0x2D4;
        register u16 *flagAddress asm("r0");
        asm("" : : "r"(flagOffset));
        flagAddress = (void *)((u8 *)actor + flagOffset);
        if (*flagAddress & mask) {
            u32 scaleOffset = 0x2C2;
            u16 *p;
            p = (void *)((u8 *)actor + scaleOffset);
            *p = 0x40;
            scale = p;
        } else {
            u16 *q = &actor->field2C2;
            u16 v = *q;
            scale = q;
            if (v == 0)
                *scale = 0x40;
        }
    }
    if (actor->drawableKind == 0)
        Anim_SetDurationScale((u8 *)actor->drawable + 0x48, *scale);
    else
        Anim_SetFrameDurationScale((u8 *)actor->drawable + 0x20, *scale);
    event = NavMesh_ConsumeProbeEvent(FIELD(nav, 0x1FC, void *));
    if (event) {
        if (SceneActorBehavior_RequestDescriptorTurn(actor, FIELD(nav, 0x1FC, void *), 0x20, 0x20,
                                                     speed)) {
            SetState(actor);
        }
    } else if (SceneActorBehavior_AccelerateTowardDescriptor(actor, FIELD(nav, 0x1FC, void *), 0x20,
                                                             0x20, speed)) {
        BeginRouteSelection(actor, nav);
        if (!InLinkMode(gEngineFlags, 0x4800)) {
            struct RouteSelection *route = actor->currentRoute;
            u8 choice;
            {
                u32 index = (gRandomIndex + 1) & 1023;
                s32 value;
                gRandomIndex = index;
                index *= 2;
                value = *(const u16 *)((u32)gRandomTable + index);
                value >>= 3;
                choice = Mod(value, route->points->count);
            }
            {
                register u32 previous asm("r2") = route->index;
                asm("" : : "r"(previous));
                if (choice == previous)
                    choice = Mod(choice + 1, route->points->count);
            }
            route->index = choice;
            route->phase = 0;
            {
                u8 *base = *(void **)((u8 *)gTerrainMap + 0x10);
                base += route->points->offset;
                route->current = base + route->index * 8;
            }
            SceneActorMovement_ResolveRoutePosition(actor);
        }
        return;
    }
    {
        register u32 offset asm("r3") = 0x2AC;
        register u32 *counter asm("r1");
        asm("" : : "r"(offset));
        counter = (void *)((u8 *)actor + offset);
        ++*counter;
    }
}
