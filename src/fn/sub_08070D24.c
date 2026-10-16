// MATCHING_CONCESSION: register-pinning
#include "scene_actor_registry.h"
static inline u32 IsUnavailable(struct SceneActorRegistryActor *input, u32 disabledInput,
                                u32 excludedInput)
{
    register struct SceneActorRegistryActor *actor asm("r3") = input;
    register u32 disabled asm("r9") = disabledInput;
    register u32 excluded asm("r8") = excludedInput;
    if (actor) {
        u32 offset = 0x2d4;
        u32 mask;
        u16 *first;
        u32 value;
        first = (void *)((u8 *)actor + offset);
        value = *first;
        mask = disabled;
        value &= mask;
        if (value)
            goto one;
        {
            u32 secondOffset = 0x2d0;
            u32 *second;
            second = (void *)((u8 *)actor + secondOffset);
            value = *second;
            mask = excluded;
            value &= mask;
            if (value)
                goto one;
        }
        if (actor->health > 0)
            goto clear;
    one:
        return 1;
    }
clear:
    return 0;
}
struct SceneActorRegistryActor *
SceneActorRegistry_FindNearestCategoryActor(const struct ContactPosition *input)
{
    const struct ContactPosition *position = input;
    struct SceneActorRegistryNode *node = gUnk_020004AC;
    register struct SceneActorRegistryActor *actor asm("r3") = node->actor;
    s32 bestDistance = 10000000;
    struct SceneActorRegistryActor *best = 0;
    if (actor) {
        register u32 disabled asm("r9") = 0x40;
        register u32 excluded asm("r8") = 0x20000;
        struct ContactPosition difference;
        struct ContactPosition *delta = &difference;
        do {
            u32 category = actor->category;
            u32 unavailable = IsUnavailable(actor, disabled, excluded);
            asm("" : "+r"(unavailable));
            if (!unavailable && (category == 4 || category == 1 || category == 3) &&
                actor->unknown2B5[0] != 21) {
                const struct ContactPosition *right = &actor->position;
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
