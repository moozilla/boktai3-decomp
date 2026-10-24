// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "scene_actor_reaction.h"
// confidence=low: starts the target-pursuit route and resets pursuit timers and callbacks.
#define FIELD(p, off, T) (*(T *)((u8 *)(p) + (off)))
void SceneBody_StartRoute(void *, void *, u32, u32, u32);
void sub_080B1FA8(void *);
void Vorn_UpdatePursuitDistance(void);
static inline u8 Test(volatile u16 *p, u32 bits)
{
    register u32 value asm("r0") = *p;
    value &= bits;
    if (value)
        return 1;
    return 0;
}
static inline void SetFlags(u16 bit, u16 *field)
{
    struct Flags {
        s16 bits : 16;
    };
    struct Flags *p = (void *)field;
    p->bits |= bit;
}

static inline void ClearFlags(u16 *field, u32 bit)
{
    struct Flags {
        s16 bits : 16;
    };
    struct Flags *p = (void *)field;
    p->bits &= ~bit;
}
static inline void SetState(struct SceneActorRegistryResetView *actor, u32 behavior)
{
    actor->state2A8 = 0;
    actor->phase2A9 = 0;
    actor->behavior2AA = behavior;
    actor->counter2AC = 0;
    actor->field2B1 = 1;
}
static inline void SetControl(struct SceneActorRegistryResetView *actor, void *control, u32 kind)
{
    actor->field2B2 = 1;
    actor->field2B0 = 0;
    actor->field2B5 = kind;
    actor->target = control;
    actor->field2C8 = 0;
    {
        u32 *flags = &actor->flags114;
        u32 mask = ~1;
        *flags &= mask;
    }
    actor->word104 = 0;
}
static inline void SetMotion(struct SceneActorRegistryResetView *actor, void *cb, u32 kind)
{
    actor->word104 = (u32)cb;
    actor->word108 = 0;
    actor->word10C = 0;
    actor->word110 = kind;
    actor->flags114 = 1;
}
static inline void StartRoute(void *body, void *path, u8 *group)
{
    u32 index = group[0x83] ^ 1;
    u8 *routes = group + 0x81;
    SceneBody_StartRoute(body, path, routes[index], 0, 0);
}
struct PursuitContext {
    u8 pad000[0x6ac];
    u16 cooldown;
};
u8 Vorn_BeginPursuitRoute(struct SceneActorRegistryResetView *actor, u32 reset)
{
    u8 *context = actor->context;
    u8 *group = context + 0x3d4;
    u16 *flags;
    if (*((u16 *)(((u8 *)context) + 0x6ac)))
        return 0;
    flags = (void *)(context + 0x6d4);
    if (((reset != 1) && Test(flags, 4)) && (!Test(flags, 2)))
        return 0;
    ((struct PursuitContext *)context)->cooldown = 60;
    {
        SetFlags(4, flags);
        ClearFlags(flags, 2);
    }
    *((u16 *)(((u8 *)context) + 0x6ec)) = 60;
    if (reset == 1)
        StartRoute(((u8 *)actor) + 0x50, group + 0x74, group);
    sub_080B1FA8(actor);
    {
        u32 *field = &actor->flags2CC;
        s32 mask = ~16;
        *field &= mask;
    }
    SetState(actor, 15);
    SetControl(actor, actor->savedTarget, 0);
    SetMotion(actor, Vorn_UpdatePursuitDistance, 37);
    {
        u16 bit = 4;
        u16 *field = (void *)(((u8 *)actor) + 0x414);
        bit |= *field;
        *field = bit;
    }
    SetFlags(0x80, &actor->flags2D4);
    {
        u32 timer = *((u16 *)(((u8 *)actor) + 0x416));
        *((u16 *)(((u8 *)actor) + 0x418)) = timer;
    }
    {
        u8 *field = ((u8 *)actor) + 0xdc;
        u32 bit = 8;
        bit |= field[15];
        field[15] = bit;
    }
    actor->flags2CC |= 0x80;
    return 1;
}
