// MATCHING_CONCESSION: register-pinning
#include "orbit_contact_actor.h"
extern void (*const gUnk_086059B4[])(struct OrbitContactActor *);
s32 OrbitContactActor_Update(struct OrbitContactActor *actor)
{
    struct ContactPosition *position;
    register struct ContactSwarmSprite *sprite asm("r12");
    struct ContactSwarmSprite *lowSprite;
    u32 angle;
    s32 radius;
    register const s16 *sine asm("r2");
    s32 value;
    s32 x;
    sub_0824923C(actor, (void *)gUnk_086059B4[actor->state]);
    if (actor->state) {
        {
            u32 spriteOffset = 0x9C;
            asm("" : "+r"(spriteOffset));
            lowSprite = (struct ContactSwarmSprite *)(spriteOffset + (u32)actor);
            sprite = lowSprite;
        }
        position = &actor->sprite.position;
        angle = actor->angle;
        radius = actor->radius;
        asm("" : : "r"(lowSprite));
        sine = gUnk_086149C4;
        value = sine[(angle + 64) & 255] * radius;
        if (value >= 0)
            x = value >> 12;
        else {
            value = -value;
            value >>= 12;
            x = -value;
        }
        {
            u32 zero = 0;
            position->x = x;
            position->y = zero;
        }
        value = sine[angle] * radius;
        asm("" : : "r"(x));
        if (value >= 0)
            position->z = value >> 12;
        else
            position->z = -((-value) >> 12);
        {
            struct ContactPosition *destination = &sprite->position;
            struct ContactPosition *center = &actor->center;
            register u32 coordinate asm("r0");
            register struct ContactSwarmSprite *firstSprite asm("r3");
            u32 delta;
            register struct ContactSwarmSprite *storeSprite asm("r5");
            register u32 lastDelta asm("r5");
            coordinate = (u16)actor->center.x;
            firstSprite = sprite;
            delta = (u16)firstSprite->position.x;
            coordinate += delta;
            storeSprite = sprite;
            storeSprite->position.x = coordinate;
            coordinate = (u16)center->y;
            delta = (u16)destination->y;
            destination->y = coordinate + delta;
            coordinate = (u16)center->z;
            lastDelta = (u16)destination->z;
            destination->z = coordinate + lastDelta;
            Collider_SetPosition(&actor->collider, destination, 0);
            Collider_Refresh(&actor->collider);
        }
    }
    return 0;
}
