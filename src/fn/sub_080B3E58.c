#include "scene_actor_reset.h"
void SceneActorAnimation_SelectDirectionalEntry(struct SceneActorRegistryResetView *, u32);
u8 Vorn_BeginContactAnimation(struct SceneActorRegistryResetView *);
void Vorn_BeginPursuitRoute(struct SceneActorRegistryResetView *, u32);
void Vorn_SelectRetreatRoute(struct SceneActorRegistryResetView *, u32);
struct VornReactionContext {
    u8 unknown0[0x6AC];
    u16 pursuitCooldown;
    u8 unknown6AE[0x12];
    u8 foreignPosition;
    u8 unknown6C1[0x13];
    u16 flags;
};
static inline u8 Test(struct VornReactionContext *context, u32 mask)
{
    if (context->flags & mask)
        return 1;
    return 0;
}
// confidence=low: pending hit flags select recovery control or pursuit/retreat behavior.
u8 Vorn_SelectHitRecovery(struct SceneActorRegistryResetView *actor)
{
    struct VornReactionContext *context = actor->context;
    u32 blocked;
    {
        u16 *flags = &context->flags;
        s32 mask = ~8;
        *flags = *flags & mask;
    }
    if (!(actor->field2D8 & 0x20000)) {
        if (actor->reactionSoundVariant == 0)
            Sound_PlaySong(0x211);
        else
            Sound_PlaySong(0x1CB);
    }
    {
        u32 mask = 0x8002;
        blocked = mask & actor->flags2D4;
    }
    if (blocked) {
        {
            u32 mask = ~0x20;
            mask &= actor->flags2D4;
            actor->flags2D4 = mask;
        }
        return 0;
    }
    if (actor->field2DC & 0xC0000) {
        void *control = actor->reactionTarget230;
        u32 kind = 0x12;
        u8 zero8;
        actor->field2B2 = 1;
        actor->field2B0 = blocked;
        actor->field2B5 = kind;
        actor->target = control;
        zero8 = 0;
        actor->field2C8 = blocked;
        {
            u32 *flags = &actor->flags114;
            s32 mask = -2;
            *flags &= mask;
        }
        actor->word104 = blocked;
        {
            u32 state = 3;
            actor->state2A8 = state;
        }
        actor->phase2A9 = zero8;
        actor->behavior2AA = zero8;
        actor->counter2AC = blocked;
        actor->field2B1 = 1;
        {
            u32 *flags = &actor->flags2D0;
            u32 mask = ~0x1000000;
            *flags &= mask;
        }
        return 1;
    }
    {
        u32 mask = ~0x20;
        actor->flags2D4 &= mask;
    }
    if (actor->flags2D4 & 0x100)
        return 0;
    if (actor->field2B8)
        return 0;
    if (Test(context, 0x1D))
        return Vorn_BeginContactAnimation((void *)context);
    if (actor->field5CA == 0 && (actor->field2E6 & 8)) {
        actor->field2B8 = 45;
        actor->field5CA = 65;
        SceneActorAnimation_SelectDirectionalEntry(actor, 3);
    }
    if (!actor->tracking.target)
        return 0;
    {
        u32 foreign = context->foreignPosition;
        if (foreign == 0) {
            context->pursuitCooldown = foreign;
            Vorn_BeginPursuitRoute(actor, 1);
        } else {
            Vorn_SelectRetreatRoute(actor, 1);
        }
        return 1;
    }
}
