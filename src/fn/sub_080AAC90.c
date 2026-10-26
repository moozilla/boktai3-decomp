// MATCHING_CONCESSION: register-pinning
#include "scene_actor_behavior.h"
#define FIELD(p, o, T) (*(T *)((u8 *)(p) + (o)))
void Collision_CalculateDamageResponse(struct SceneActorBehaviorContactView *,
                                       struct SceneActorBehaviorContactView *);
void TentacleActor_EnterHitReaction(void *);
s32 Math_Atan2(s32, s32);
static inline u32 Unavailable(struct SceneActorBehaviorView *actor)
{
    u32 result;
    if (actor) {
        u32 mask = 0x40;
        u32 offset = 0x2D4;
        u32 value;
        value = *(u16 *)((u32)actor + offset);
        if (value & mask)
            goto unavailable;
        mask = 0x20000;
        {
            u32 offset = 0x2D0;
            value = *(u32 *)((u32)actor + offset);
        }
        if (value & mask)
            goto unavailable;
        if (actor->health > 0)
            goto available;
    unavailable:
        result = 1;
        goto done;
    }
available:
    result = 0;
done:
    asm("" : "+r"(result));
    return result;
}
void TentacleActor_ReactToContact(struct SceneActorBehaviorContactView *source,
                                  struct SceneActorBehaviorContactView *destination, void *input)
{
    void *actor = input;
    register void *self asm("r8") = actor;
    struct SceneActorBehaviorView *target = FIELD(actor, 0x124, void *);
    if (!Unavailable(target)) {
        struct ContactPosition difference;
        struct ContactPosition *delta = &difference;
        struct ContactPosition *right = (void *)((u8 *)destination + 0x24);
        struct ContactPosition *left = (void *)((u8 *)source + 0x24);
        u32 table;
        u32 distance;
        register long long direction asm("r1");
        difference.x = (u16)left->x - (u16)right->x;
        delta->y = (u16)left->y - (u16)right->y;
        table = (u32)gUnk_0203B400;
        delta->z = (u16)left->z - (u16)right->z;
        distance = difference.x * difference.x + difference.z * difference.z;
        if (distance < FIELD(target, 0x8EC, u32)) {
            Collision_CalculateDamageResponse(source, destination);
            destination->parameter44 = source->parameter44;
            destination->power = source->power;
            if (source->flags & 0x100) {
                direction = source->direction;
                goto store;
            }
            if (!difference.x && !difference.z) {
                gRandomIndex = (gRandomIndex + 1) & 0x3FF;
                direction = *(const u16 *)(gRandomIndex * 2 + table);
            store:
                FIELD(self, 0x108, u8) = direction;
            } else {
                FIELD(self, 0x108, u8) = Math_Atan2(difference.x, difference.z);
            }
            FIELD(target, 0x8EC, u32) = distance;
            FIELD(target, 0x8F0, void *) = actor;
        }
        TentacleActor_EnterHitReaction(target);
    }
}
