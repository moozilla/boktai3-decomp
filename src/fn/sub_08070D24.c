#include "scene_actor_registry.h"
static inline s16 IsUnavailable(struct ChaseActor *input, u32 disabledInput, u32 excludedInput)
{
    struct ChaseActor *actor = input;
    u32 disabled = disabledInput;
    u32 excluded = excludedInput;
    if (actor) {
        u32 mask;
        u16 *first;
        u32 value;
        first = &actor->base.motionFlags;
        value = *first;
        mask = disabled;
        value &= mask;
        if (value)
            goto one;
        {
            u32 *second;
            second = &actor->base.statusFlags;
            value = *second;
            mask = excluded;
            value &= mask;
            if (value)
                goto one;
        }
        if (actor->base.health > 0)
            goto clear;
    one:
        return 1;
    }
clear:
    return 0;
}
struct ChaseActor *SceneActorRegistry_FindNearestCategoryActor(const struct ContactPosition *input)
{
    const struct ContactPosition *position = input;
    struct SceneActorRegistryNode *node = gSceneActorRegistryNode;
    struct ChaseActor *actor = node->actor;
    s32 bestDistance = 10000000;
    struct ChaseActor *best = 0;
    if (actor) {
        u32 disabled = 0x40;
        u32 excluded = 0x20000;
        struct ContactPosition difference;
        struct ContactPosition *delta = &difference;
        do {
            u32 category = actor->base.category;
            u32 unavailable = IsUnavailable(actor, disabled, excluded);

            if (!unavailable && (category == 4 || category == 1 || category == 3) &&
                actor->base.controlKind != 21) {
                const struct ContactPosition *right = &actor->base.body.position;
                s32 x;
                s32 z;
                s32 xSquare;
                s32 zSquare;
                s32 distance;
                delta->x = (u16)position->x - (u16)right->x;
                delta->y = (u16)position->y - (u16)right->y;
                delta->z = (u16)position->z - (u16)right->z;
                x = delta->x;
                xSquare = x * x;
                z = delta->z;
                zSquare = z * z;
                {
                    s32 copy = zSquare;
                    distance = (s32)((u32)xSquare + (u32)copy);
                }
                if (bestDistance > distance) {
                    bestDistance = distance;
                    best = actor;
                }
            }
            node = node->next;
            actor = node->actor;
        } while (actor);
    }
    return best;
}
