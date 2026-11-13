#include "explosion.h"

static inline void UpdateFrame(struct Explosion *explosion)
{
    explosion->sprite.frame = explosion->age >> 2;
}

s32 Explosion_Update(struct Explosion *explosion)
{
    u16 *delay = &explosion->delay;
    if (*delay)
        (*delay)--;
    else {
        u16 *age;
        u32 value;
        UpdateFrame(explosion);
        value = explosion->age;
        age = &explosion->age;
        if (value == 0) {
            explosion->sprite.flags &= ~1;
            if (explosion->silent == 0)
                Sound_PlaySong(0x195);
        }
        if (*age <= 7 && explosion->collisionDisabled == 0)
            Collider_Refresh(&explosion->collider);
        (*age)++;
        if (*age > 23)
            Task_Kill(explosion);
    }
    return 0;
}

u32 Explosion_Destroy(struct Explosion *explosion)
{
    Sprite_Remove(&explosion->sprite);
    return 0;
}

static inline void SetSpriteHidden(struct ContactSwarmSprite *sprite, u32 bit)
{
    sprite->priority = bit;
    sprite->flags |= bit;
}

void Explosion_InitSprite(struct ContactSwarmSprite *sprite, struct ContactSwarmGraphics *resource,
                          struct ContactPosition *position, s32 palette)
{
    AnimData_Load(resource, 0x8639);
    Sprite_Init(sprite, resource, 0);
    sprite->frame = 0;
    palette += 0x116;
    AnimData_SelectPaletteBank(resource, palette);
    sprite->position = *position;
    SetSpriteHidden(sprite, 1);
}

void Explosion_InitCollider(struct Explosion *explosion, struct ContactCollider *collider,
                            u32 contactFlags, s32 power, s32 force, s32 reactionFlags,
                            s32 hitDuration)
{
    struct ExplosionSize size, offset;

    size.x = 120;
    size.y = 120;
    size.z = 120;
    *(u32 *)&offset = 0;
    ((u32 *)&offset)[1] &= 0xFFFF0000;
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
    *(u32 *)&offset = 0;
    ((u32 *)&offset)[1] &= 0xFFFF0000;
    Collider_Init(collider, 0, 0x2001, 0, (u16)contactFlags, &size, &offset);
    Collider_SetAttackProperties(collider, power, force, 0x200 | collisionProperties, reactionFlags,
                                 hitDuration);
    Collider_SetCallback(collider, 0, explosion);
    if (power == 0 && force == 0)
        explosion->collisionDisabled = 1;
}

static inline void InitializeDelay(u16 *dest, u32 zero, u32 value)
{
    *dest++ = value;
    *dest = zero;
}

s32 Explosion_Init(struct Explosion *explosion, struct ContactPosition *position, u32 contactFlags,
                   s32 power, s32 force, s32 reactionFlags, s32 hitDuration, s32 palette, u32 delay)
{
    struct ContactCollider *collider;
    Explosion_InitSprite(&explosion->sprite, &explosion->graphics, position, palette);
    collider = &explosion->collider;
    Explosion_InitCollider(explosion, collider, contactFlags, power, force, reactionFlags | 0x40000,
                           hitDuration);
    Collider_SetPosition(collider, position, 0);
    InitializeDelay(&explosion->delay, 0, delay);
    return 0;
}

s32 Explosion_InitWithCollisionFlags(struct Explosion *explosion, struct ContactPosition *position,
                                     u32 contactFlags, s32 collisionProperties, s32 power,
                                     s32 force, s32 reactionFlags, s32 hitDuration, s32 palette,
                                     u32 delay)
{
    struct ContactCollider *collider;
    Explosion_InitSprite(&explosion->sprite, &explosion->graphics, position, palette);
    collider = &explosion->collider;
    Explosion_InitColliderWithFlags(explosion, collider, contactFlags, collisionProperties, power,
                                    force, reactionFlags | 0x40000, hitDuration);
    Collider_SetPosition(collider, position, 0);
    InitializeDelay(&explosion->delay, 0, delay);
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
