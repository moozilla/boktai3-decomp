#include "objects/bouncing_bombs/bouncing_object.h"
#include "scene/motion/scene_actor_terrain.h"
static inline s32 Truncate(s32 product)
{
    if (product >= 0)
        return product >> 12;
    return -((-product) >> 12);
}
static inline void Vector(struct ContactPosition *result, s32 angle, s32 magnitude)
{
    result->x = Truncate(gUnk_086149C4[(angle + 64) & 255] * magnitude);
    result->y = 0;
    result->z = Truncate(gUnk_086149C4[angle & 255] * magnitude);
}
void BouncingBomb_ApplyContactImpulse(struct BouncingContact *a, struct BouncingContact *b,
                                      struct BouncingObject *object)
{
    struct ContactPosition delta;
    struct ContactPosition impulse;
    struct ContactPosition *out;
    const struct ContactPosition *from;
    s32 angle;

    out = &delta;
    from = &b->position;
    SceneActorTerrain_Subtract(out, &a->position, from);
    angle = Math_Atan2(delta.x, delta.z);
    angle = (((angle + 16) & 255) >> 5) << 5;
    if (delta.y > 0) {
        Vector(&impulse, angle, 4);
        impulse.y = -4;
        object->velocity.x = impulse.x;
        object->velocity.y = impulse.y;
        object->velocity.z = impulse.z;
    } else {
        Vector(&impulse, angle, 18);
        impulse.y = 32;
        object->velocity.x += impulse.x;
        object->velocity.y += impulse.y;
        object->velocity.z += impulse.z;
    }
    object->sprite.position.y++;
    object->collisionDelay = 12;
    Sound_PlaySong(0x166);
}
