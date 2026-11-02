#include "global.h"
#include "scene_actor_reaction.h"
// confidence=low: starts the target-pursuit route and resets pursuit timers and callbacks.
void SceneBody_StartRoute(void *, void *, u32, u32, u32);
void sub_080B1FA8(void *);
void Vorn_UpdatePursuitDistance(void);
struct VornResponseFlags {
    s16 bits : 16;
};
static inline u8 Vorn_HasPursuitFlag(struct VornResponseFlags *const *cursor, u16 mask)
{
    u16 value = (*cursor)->bits;
    value &= mask;
    if (value)
        return 1;
    return 0;
}
static inline u8 Vorn_TestResponseFlags(struct VornResponseFlags *flags, u32 mask)
{
    u16 value = flags->bits;
    if (value & mask)
        return 1;
    return 0;
}
static inline void Vorn_SetResponseFlags(struct VornResponseFlags *flags, u16 bits)
{
    flags->bits |= bits;
}
static inline void SetFlags(u16 bit, u16 *field)
{
    struct Flags {
        s16 bits : 16;
    };
    struct Flags *p = (void *)field;
    p->bits |= bit;
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
u8 Vorn_BeginPursuitRoute(struct SceneActorRegistryResetView *actor, u32 reset)
{
    struct SceneActorRegistryResetView *context = actor->context;
    u8 *group = (void *)&context->group.common;
    struct VornResponseFlags *flags;
    if (context->field6AC)
        return 0;
    flags = (struct VornResponseFlags *)&context->flags6D4;
    if (reset != 1 && Vorn_HasPursuitFlag(&flags, 4) && !Vorn_TestResponseFlags(flags, 2))
        return 0;
    context->field6AC = 60;
    {
        Vorn_SetResponseFlags(flags, 4);
        flags->bits &= ~2;
    }
    *((u16 *)(((u8 *)context) + 0x6ec)) = 60;
    if (reset == 1)
        StartRoute(&actor->body.id, group + 0x74, group);
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
        u16 *field = &actor->group.common.gate.flags;
        bit |= *field;
        *field = bit;
    }
    SetFlags(0x80, &actor->flags2D4);
    {
        u32 timer = actor->group.common.gate.duration;
        actor->group.common.gate.counter = timer;
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
