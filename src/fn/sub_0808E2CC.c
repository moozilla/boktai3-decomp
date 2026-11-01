#include "global.h"

#include "scene_actor_chase.h"
// confidence=low: terrain-edge response is registered as the Slime body collider callback.
void SceneActorBehavior_ApplyContactReaction(struct SceneActorBehaviorContactView *,
                                             struct SceneActorBehaviorContactView *,
                                             struct SceneActorBehaviorView *);
void SceneActorMotion_ArmPrimaryCollider(void *);
struct SlimeContact {
    u8 unknown0[6];
    u16 flags;
    u8 unknown8[0x1C];
    struct ContactPosition position;
};
static inline u8 Unavailable(struct SceneActorRegistryResetView *actor)
{
    if (actor) {
        u32 mask = 0x40;
        if (actor->flags2D4 & mask)
            return 1;
        mask = 0x20000;
        if (actor->flags2D0 & mask)
            return 1;
        if (actor->health <= 0)
            return 1;
    }
    return 0;
}
static inline void Add(struct ContactPosition *out, struct ContactPosition *a,
                       struct ContactPosition *b)
{
    out->x = b->x + a->x;
    out->y = b->y + a->y;
    out->z = b->z + a->z;
}
static inline void Subtract(struct ContactPosition *out, struct ContactPosition *a,
                            struct ContactPosition *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}
static inline s32 DivideAngle(s32 angle)
{
    return angle >= 0 ? angle >> 6 : -(-angle >> 6);
}
static inline u32 ContactFlags(struct SlimeContact *contact, u32 mask)
{
    return contact->flags & mask;
}
void Slime_ResolveBodyContact(struct SlimeContact *contact, void *other,
                              struct SceneActorRegistryResetView *actor)
{
    struct ContactPosition next, displacement, center, fromContact, adjusted;
    s32 angle;
    u32 contactFlags;
    struct SceneActorRegistryResetView *checkActor = actor;
    if (Unavailable(checkActor))
        return;
    contactFlags = ContactFlags(contact, 0x800);

    if (contactFlags) {
        Add(&next, &actor->body.position, &actor->body.motion);
        Subtract(&displacement, &contact->position, &next);
        SceneActorTerrain_GetCenter(actor->terrainCell, &center);
        Subtract(&fromContact, &center, &contact->position);
        angle = DivideAngle(Math_Atan2(fromContact.x, fromContact.z) + 32);
        switch (angle) {
            case 0:
                displacement.x += 0xC0;
                displacement.z = angle;
                break;
            case 1:
                displacement.x = 0;
                displacement.z += 0xC0;
                break;
            case 2:
                displacement.x -= 0xC0;
                displacement.z = 0;
                break;
            case 3:
                displacement.x = 0;
                displacement.z -= 0xC0;
                break;
        }
        Add(&adjusted, &actor->body.position, &displacement);
        {
            s32 cell = Chase_CellAt(&adjusted);
            u32 ground = actor->groundHeight2BE;
            u8 *data = SceneActorTerrain_GetCell(cell);
            if (ground != (*data & 15) << 8)
                SceneActorMotion_ArmPrimaryCollider(actor);
            else {
                actor->body.position.x += displacement.x;
                actor->body.position.z += displacement.z;
                actor->body.motion.x = 0;
                actor->body.motion.z = 0;
            }
        }
    } else
        SceneActorBehavior_ApplyContactReaction((void *)contact, other, (void *)checkActor);
}
