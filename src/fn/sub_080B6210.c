#define gSineTable gUnk_086149C4
/* Descriptive aliases and object roles are tentative (confidence: low). */
#include "global.h"
#include "skeleton.h"
#include "scene_actor_reset.h"
static inline u32 ActorFlag(struct ChaseActor *actor, u32 input, u32 offset)
{
    u32 mask = input;
    return offset == 0x2D4 ? *(u16 *)((u8 *)actor + offset) & mask
                           : *(u32 *)((u8 *)actor + offset) & mask;
}
static inline u8 unavailable(struct ChaseActor *actor)
{
    if (actor && (ActorFlag(actor, 0x40, 0x2D4) || ActorFlag(actor, 0x20000, 0x2D0) ||
                  actor->base.health <= 0))
        return 1;
    return 0;
}
static inline s32 scaled(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -(-x >> 12);
}

// confidence=low: updates Skeleton body and facing attack bounds while its control is active
static inline void SetBounds(struct ContactCollider *collider,
                             union SceneActorRegistryPackedPosition *bounds)
{
    collider->bounds = *(struct ContactPosition *)bounds;
}
void Skeleton_UpdateFacingColliders(struct ChaseActor *p)
{
    struct Skeleton *context = p->base.context;
    union SceneActorRegistryPackedPosition pos, bounds;
    u32 mask;
    if (unavailable(p))
        return;
    mask = 0x800;
    if (p->base.flags2D4 & mask)
        return;
    if (p->base.field2B5 == 0x39)
        return;
    if (p->base.field2B5 == 0x3D)
        return;
    if (context->collisionDelay) {
        p->base.colliderA.flags |= 4;
        context->bodyCollider.flags |= 4;
        context->collisionDelay--;
    } else {
        p->base.colliderA.flags &= ~4;
        context->bodyCollider.flags &= ~4;
    }
    pos.position = p->base.body.position;
    Collider_SetPosition(&context->bodyCollider, &pos, 0);
    pos.packed.x += scaled(gUnk_086149C4[(p->base.body.direction + 64) & 255] * 128);
    pos.packed.z += scaled(gUnk_086149C4[p->base.body.direction] * 128);
    switch (((p->base.body.direction + 16) & 255) >> 6) {
        case 0:
        case 2:
            if (p->base.category == 6) {
                bounds.packed.x = 180;
                bounds.packed.z = 84;
            } else {
                bounds.packed.x = 160;
                bounds.packed.z = 64;
            }
            break;
        case 1:
        case 3:
            if (p->base.category == 6) {
                bounds.packed.x = 84;
                bounds.packed.z = 180;
            } else {
                bounds.packed.x = 64;
                bounds.packed.z = 160;
            }
            break;
    }
    bounds.packed.y = 100;
    SetBounds(&context->attackCollider, &bounds);
    Collider_SetPosition(&context->attackCollider, &pos, 0);
    Collider_SetPosition(&context->bodyCollider, &pos, 0);
}
