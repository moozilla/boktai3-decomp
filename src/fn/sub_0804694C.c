#include "orbit_contact_actor.h"
extern void (*const gUnk_086059B4[])(struct OrbitContactActor *);
static inline s32 scaleOrbit(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
s32 OrbitContactActor_Update(struct OrbitContactActor *actor)
{
    gUnk_086059B4[actor->state](actor);
    if (actor->state) {
        struct ContactSwarmSprite *sprite = &actor->sprite;
        struct ContactPosition *position = &actor->sprite.position;
        u32 angle = actor->angle;
        s32 radius = actor->radius;
        position->x = scaleOrbit(gUnk_086149C4[(angle + 64) & 255] * radius);
        position->y = 0;
        position->z = scaleOrbit(gUnk_086149C4[angle] * radius);
        {
            struct ContactPosition *destination = &sprite->position;
            const struct ContactPosition *center = &actor->center;
            sprite->position.x += actor->center.x;
            destination->y += center->y;
            destination->z += center->z;
            Collider_SetPosition(&actor->collider, destination, 0);
            Collider_Refresh(&actor->collider);
        }
    }
    return 0;
}
