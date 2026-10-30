#include "scene_actor_chase_reaction.h"
#include "scene_actor_effects.h"

struct DogItemContext {
    u8 unknown0[0x6A0];
    s16 slot, kind, parameter;
    u8 turnClockwise, blockedRoutes, unknown6A8, healthPercent;
};

u8 InventorySlot_PickRandom(s16 *, s16 *, s16 *);
s32 FloatingItemEffect_Spawn(s32, s32, s32, s32, void *, void *, s32, void *);
void Inventory_ClearItemSlot(u32);
s32 Math_Atan2(s32, s32);

static inline u8 Dog_TracksPlayer(struct SceneActorRegistryResetView *actor)
{
    u32 mask = 2;
    if (actor->flags2C0 & mask)
        return 1;
    return 0;
}

static inline void Dog_ClearResponseFlags(void *object)
{
    struct SceneActorRegistryResetView *actor = object;
    actor->field2D8 = 0;
    actor->field2DC = 0;
}

static inline u32 *Dog_ResponseField(void *object, s16 offset)
{
    return (u32 *)((u8 *)object + offset);
}
static inline void Dog_WriteResponseWords(struct SceneActorRegistryResetView *actor, s32 value)
{
    *Dog_ResponseField(actor, (u32)&((struct SceneActorRegistryResetView *)0)->field2D8) = value;
    *Dog_ResponseField(actor, (u32)&((struct SceneActorRegistryResetView *)0)->field2DC) = value;
}

static inline void Dog_CopyContactCost(struct ContactCollider *collider,
                                  struct SceneActorRegistryResetView *actor)
{
    collider->cost = actor->reactionSoundVariant;
}

void sub_080DF6EC(void *unused, struct ContactCollider *contact,
                  struct SceneActorRegistryResetView *actor)
{
    struct DogItemContext *context;
    u32 palette;
    struct SceneActorEffectsOffset position;
    struct ContactPosition *actorPosition;
    s32 responseValue, spawned, health;
    if (SceneActorChase_Test(gEngineFlags, 0x4800))
        return;
    if (contact->contactFlags != 1)
        return;
    context = actor->context;
    palette = actor->paletteOffset;
    if (context->slot >= 0)
        return;
    if (palette != 0 && palette != 2)
        return;
    position = *(struct SceneActorEffectsOffset *)&((struct SceneActorChasePlayer *)gUnknown_02000580[0])->target.position;
    position.y += 200;
    if (!InventorySlot_PickRandom(&context->slot, &context->kind, &context->parameter))
        return;
    spawned = FloatingItemEffect_Spawn(0, context->kind, 3, 0, &position, &actor->body.position, 24, actor);
    actorPosition = &actor->body.position;
    if (spawned < 0) {
        SceneActorChase_SetFlagComponent((struct SceneActorChaseFlags16 *)&context->slot, 0xFFFF);
        return;
    }
    Inventory_ClearItemSlot(context->slot);
    switch (context->kind) {
        case 2:
        case 5:
        case 14:
            break;
        default:
            return;
    }
    responseValue = Div((u16)actor->defaultHealth * context->healthPercent, 100);
    {
        health = actor->health;
        if (health - responseValue <= 0)
            actor->reactionSoundVariant = health;
        else
            actor->reactionSoundVariant = responseValue;
    }
    context->slot = -1;
    actor->tracking.target = &((struct SceneActorChasePlayer *)gUnknown_02000580[0])->target;
    actor->field2E0 = 1;
    Dog_ClearResponseFlags(actor);
    responseValue = 0;
    Dog_WriteResponseWords(actor, responseValue);
    Dog_CopyContactCost(&actor->colliderA, actor);
    actor->colliderA.responseParameter = 1;
    actor->colliderA.responseValue = 30;
    actor->field2E6 = 1;
    if (Dog_TracksPlayer(actor))
        actor->tracking.target = &((struct SceneActorChasePlayer *)gUnknown_02000580[0])->target;
    else if (!actor->tracking.target)
        return;
    {
        struct SceneActorChaseTarget *target = actor->tracking.target;
        s32 dx = target->position.x - actorPosition->x;
        s32 dz = target->position.z - actor->body.position.z;
        actor->tracking.heading = Math_Atan2(dx, dz);
        actor->tracking.distance = dx * dx + dz * dz;
    }
}
