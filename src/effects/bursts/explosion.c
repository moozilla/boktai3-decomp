#include "effects/bursts/explosion.h"

s32 Explosion_Update(struct Explosion *explosion)
{
    if (explosion->delay) {
        explosion->delay--;
    } else {
        explosion->sprite.frame = explosion->age >> 2;
        if (explosion->age == 0) {
            explosion->sprite.flags &= ~1;
            if (!explosion->silent)
                Sound_PlaySong(0x195);
        }
        if (explosion->age <= 7 && !explosion->collisionDisabled)
            Collider_Refresh(&explosion->collider);
        explosion->age++;
        if (explosion->age > 23)
            Task_Kill(explosion);
    }
    return 0;
}

u32 Explosion_Destroy(struct Explosion *explosion)
{
    Sprite_Remove(&explosion->sprite);
    return 0;
}

void Explosion_InitSprite(struct ContactSwarmSprite *sprite, struct ContactSwarmGraphics *resource,
                          struct ContactPosition *position, s32 palette)
{
    AnimData_Load(resource, 0x8639);
    Sprite_Init(sprite, resource, 0);
    sprite->frame = 0;
    AnimData_SelectPaletteBank(resource, palette + 0x116);
    sprite->position = *position;
    sprite->priority = 1;
    sprite->flags |= 1;
}

void Explosion_InitCollider(struct Explosion *explosion, struct ContactCollider *collider,
                            u32 contactFlags, s32 power, s32 force, s32 reactionFlags,
                            s32 hitDuration)
{
    struct ExplosionSize size, offset;

    size.x = 120;
    size.y = 120;
    size.z = 120;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    Collider_Init(collider, 0, 0x2001, 0, (u16)contactFlags, &size, &offset);
    Collider_SetAttackProperties(collider, power, force, 0x210, reactionFlags, hitDuration);
    Collider_SetCallback(collider, 0, explosion);
    if (power == 0 && force == 0)
        explosion->collisionDisabled = 1;
}

void Explosion_InitColliderWithFlags(struct Explosion *explosion, struct ContactCollider *collider,
                                     u32 contactFlags, s32 collisionProperties, s32 power,
                                     s32 force, s32 reactionFlags, s32 hitDuration)
{
    struct ExplosionSize size, offset;

    size.x = 120;
    size.y = 120;
    size.z = 120;
    offset.x = 0;
    offset.y = 0;
    offset.z = 0;
    Collider_Init(collider, 0, 0x2001, 0, (u16)contactFlags, &size, &offset);
    Collider_SetAttackProperties(collider, power, force, 0x200 | collisionProperties, reactionFlags,
                                 hitDuration);
    Collider_SetCallback(collider, 0, explosion);
    if (power == 0 && force == 0)
        explosion->collisionDisabled = 1;
}

s32 Explosion_Init(struct Explosion *explosion, struct ContactPosition *position, u32 contactFlags,
                   s32 power, s32 force, s32 reactionFlags, s32 hitDuration, s32 palette, u32 delay)
{
    Explosion_InitSprite(&explosion->sprite, &explosion->graphics, position, palette);
    Explosion_InitCollider(explosion, &explosion->collider, contactFlags, power, force,
                           reactionFlags | 0x40000, hitDuration);
    Collider_SetPosition(&explosion->collider, position, 0);
    explosion->delay = delay;
    explosion->age = 0;
    return 0;
}

s32 Explosion_InitWithCollisionFlags(struct Explosion *explosion, struct ContactPosition *position,
                                     u32 contactFlags, s32 collisionProperties, s32 power,
                                     s32 force, s32 reactionFlags, s32 hitDuration, s32 palette,
                                     u32 delay)
{
    Explosion_InitSprite(&explosion->sprite, &explosion->graphics, position, palette);
    Explosion_InitColliderWithFlags(explosion, &explosion->collider, contactFlags,
                                    collisionProperties, power, force, reactionFlags | 0x40000,
                                    hitDuration);
    Collider_SetPosition(&explosion->collider, position, 0);
    explosion->delay = delay;
    explosion->age = 0;
    return 0;
}

/* Explosion_CreateImmediate: Allocates an explosion with default collision parameters and zero activation delay */

struct Explosion *Explosion_CreateImmediate(struct ContactPosition *position, u32 contactFlags,
                                            s32 power, s32 force, s32 palette)
{
    struct Explosion *explosion = Task_Create(10, 188);
    if (explosion) {
        Task_SetCallbacks(explosion, Explosion_Update, Explosion_Destroy);
        if (Explosion_Init(explosion, position, contactFlags, power, force, 262144, 60, palette,
                           0) < 0) {
            Task_Kill(explosion);
            return 0;
        }
    }
    return explosion;
}

/* Explosion_CreateDelayed: Allocates an explosion with default collision parameters and a caller-supplied activation delay */

struct Explosion *Explosion_CreateDelayed(struct ContactPosition *position, u32 contactFlags,
                                          s32 power, s32 force, s32 palette, u32 delay)
{
    struct Explosion *explosion = Task_Create(10, 188);
    if (explosion) {
        Task_SetCallbacks(explosion, Explosion_Update, Explosion_Destroy);
        if (Explosion_Init(explosion, position, contactFlags, power, force, 262144, 60, palette,
                           delay) < 0) {
            Task_Kill(explosion);
            return 0;
        }
    }
    return explosion;
}

struct Explosion *Explosion_Create(struct ContactPosition *position, u32 contactFlags, s32 power,
                                   s32 force, s32 reactionFlags, s32 hitDuration, s32 palette,
                                   u32 delay)
{
    struct Explosion *explosion = Task_Create(10, 188);
    if (explosion) {
        Task_SetCallbacks(explosion, Explosion_Update, Explosion_Destroy);
        if (Explosion_Init(explosion, position, contactFlags, power, force, reactionFlags,
                           hitDuration, palette, delay) < 0) {
            Task_Kill(explosion);
            return 0;
        }
    }
    return explosion;
}

struct Explosion *Explosion_CreateAt(struct ContactPosition *position, u32 contactFlags, s32 power,
                                     s32 force, s32 reactionFlags, s32 hitDuration, s32 palette,
                                     u32 delay)
{
    struct Explosion *explosion = Task_Create(10, 188);
    if (explosion) {
        Task_SetCallbacks(explosion, Explosion_Update, Explosion_Destroy);
        if (Explosion_Init(explosion, position, contactFlags, power, force, reactionFlags,
                           hitDuration, palette, delay) < 0) {
            Task_Kill(explosion);
            return 0;
        }
    }
    explosion->silent = 1;
    return explosion;
}

struct Explosion *Explosion_CreateWithCollisionFlags(struct ContactPosition *position,
                                                     u32 contactFlags, s32 collisionProperties,
                                                     s32 power, s32 force, s32 reactionFlags,
                                                     s32 hitDuration, s32 palette, u32 delay)
{
    struct Explosion *explosion = Task_Create(10, 188);
    if (explosion) {
        Task_SetCallbacks(explosion, Explosion_Update, Explosion_Destroy);
        if (Explosion_InitWithCollisionFlags(explosion, position, contactFlags, collisionProperties,
                                             power, force, reactionFlags, hitDuration, palette,
                                             delay) < 0) {
            Task_Kill(explosion);
            return 0;
        }
    }
    return explosion;
}
