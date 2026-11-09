#include "global.h"

// confidence=low: animation checkpoint emits aimed shots with rising speed and falling strength.

#include "scene_actor_reset.h"
#include "engine.h"

s32 Div(s32, s32);
void BouncingObject_Spawn(struct ContactPackedVector *, struct ContactPosition *,
                          struct ContactPosition *, u32, u32, u32, u32);
extern const s16 gUnk_086149C4[];
static inline s32 Scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}
static inline void Subtract(struct ContactPosition *out, struct ContactPosition *from,
                            struct ContactPosition *to)
{
    out->x = to->x - from->x;
    out->y = to->y - from->y;
    out->z = to->z - from->z;
}
static inline s32 Abs(s32 value)
{
    return value < 0 ? -value : value;
}
void Mummy_FireAcceleratingVolley(struct SceneActorRegistryResetView *actor)
{
    u8 *body = actor->context;
    struct ContactPackedVector source = *(struct ContactPackedVector *)&actor->body.position;
    struct ContactPosition delta;
    struct ContactPosition *velocity;
    s32 divisor, facing, difference;
    s32 heightDifference;
    s32 speed;
    s32 count, i;
    s32 direction;
    s32 strength;
    source.y += 200;
    heightDifference = source.y - actor->tracking.target->position.y;
    if (heightDifference <= 255)
        divisor = 15;
    else if (heightDifference <= 511)
        divisor = 20;
    else if (heightDifference <= 767)
        divisor = 23;
    else if (heightDifference <= 1023)
        divisor = 27;
    else
        divisor = 34;
    Subtract(&delta, &actor->body.position, &actor->tracking.target->position);
    {
        s32 x = delta.x, z;
        s32 ax = Abs(x), az;
        z = delta.z;
        az = Abs(z);
        if (ax > az)
            speed = Div(x, divisor);
        else
            speed = Div(z, divisor);
        speed = Abs(speed);
    }
    if (speed <= 10)
        speed = 10;
    if (speed > *(u16 *)(body + 0x6cc))
        speed = *(u16 *)(body + 0x6cc);
    switch (actor->paletteOffset) {
        case 0:
            count = 1;
            break;
        case 1:
            count = 2;
            break;
        default:
            count = 3;
            break;
    }
    facing = ((actor->body.direction + 32) & 255) >> 6 << 6;
    difference = actor->tracking.heading - facing;
    if (difference > 128)
        difference -= 256;
    else if (difference < -128)
        difference += 256;
    direction = difference;
    if (Abs(direction) > 28)
        direction = facing;
    else
        direction = actor->tracking.heading;
    i = count;
    strength = *(u16 *)(body + 0x6ce);
    for (; i != 0; i--) {
        velocity = &delta;
        velocity->x = Scale(gUnk_086149C4[(direction + 64) & 255] * speed);
        velocity->y = 0;
        velocity->z = Scale(gUnk_086149C4[direction & 255] * speed);
        {
            u32 shot = actor->attacks.first[2];
            u32 category = actor->attacks.second[2];
            void *target = &actor->tracking.target->position;
            BouncingObject_Spawn(&source, &delta, target, shot, category, strength, actor->body.id);
        }
        strength = Div(strength << 3, 10);
        speed += Div(speed << 2, 10);
    }
    Sound_PlaySong(0x101);
}
