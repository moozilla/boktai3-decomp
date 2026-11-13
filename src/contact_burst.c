#include "contact_burst.h"

#include "projected_display.h"

#include "directional_particles.h"

#include "render.h"

void ContactBurst_OnContact(struct ContactCollider *self, struct ContactCollider *otherInput,
                            struct ContactBurst *effectInput)
{
    struct ContactCollider *other = otherInput;
    struct ContactBurst *effect = effectInput;
    u32 direction;
    do {
        effect->contactTimer = 18;
    } while (0);
    {
        u32 mask = 256;
        if (self->flags & mask)
            direction = self->direction;
        else
            direction = Math_Atan2(other->x - self->x, other->z - self->z);
    }
    effect->contactDirection = direction;
}

static inline s32 nextRandomHighByte(void)
{
    s32 sample;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    sample = ((const u16 *)RANDOM_TABLE_ADDR)[gRandomIndex];
    return sample >> 8;
}

void ContactBurst_UpdateContactAnimation(struct ContactBurst *effect)
{
    s32 phase = effect->frame & 7;
    if (effect->contactTimer) {
        u8 *contactFrame;
        if (!(effect->contactTimer & 7)) {
            s32 sample;
            gRandomIndex = (gRandomIndex + 1) & 1023;
            sample = ((const u8 *)RANDOM_TABLE_ADDR)[gRandomIndex * 2];
            if (sample <= 127)
                Sound_PlaySong(0x1BF);
            else
                Sound_PlaySong(0x1C0);
        }
        contactFrame = &effect->contactFrame;
        if (!phase)
            *contactFrame = Mod(nextRandomHighByte(), 3) * 2 + 32;
        if (phase <= 3) {
            u32 frame = *contactFrame;
            struct TransientEffectSprite *sprite = &effect->sprite;
            sprite->frame = frame;
        } else {
            u32 frame = *contactFrame;
            struct TransientEffectSprite *sprite = &effect->sprite;
            sprite->frame = frame + 1;
        }
        --effect->contactTimer;
    } else {
        if (phase <= 3) {
            struct TransientEffectSprite *sprite = &effect->sprite;
            sprite->frame = 27;
        } else {
            struct TransientEffectSprite *sprite = &effect->sprite;
            sprite->frame = 28;
        }
    }
    ++effect->frame;
}

u32 ContactBurst_RandomParticleFrame(void)
{
    return Mod(nextRandomHighByte(), 3) + 29;
}

s32 DirectionalParticle_Spawn(struct DirectionalParticleSource *, u32, u32);

static inline void projectPosition(struct ContactPosition *screen,
                                   const struct ContactPosition *world)
{
    s32 x = world->x;
    s32 z = world->z;
    s32 scaledSum;
    s32 sum;
    s32 vertical, y, depth;
    screen->x = projectScale(3 * (x - z));
    scaledSum = projectScale(3 * (x + z));
    sum = scaledSum;
    vertical = projectScale(3 * world->y);
    y = sum - vertical;
    depth = sum + vertical;
    screen->x = (u16)screen->x - gCameraOrigin[0] + 120;
    screen->y = y - gCameraOrigin[1] + 80;
    screen->z = depth - gCameraOrigin[2];
}

s32 ContactBurst_Update(struct ContactBurst *effect)
{
    if (effect->enabled) {
        struct ContactPosition *screen;
        if (!effect->previousEnabled) {
            effect->fade = 1;
            Sound_PlaySong(0x3B6);
        }
        effect->sprite.flags &= ~1;
        screen = &effect->sprite.position.vector;
        projectPosition(screen, effect->world);
        {
            struct ContactBurst *addressBase = effect;
            s16 *height;
            height = &addressBase->sprite.position.vector.z;
            *height += 50;
        }
        if (effect->contactTimer) {
            struct ContactCollider *collider = &effect->collider;
            collider->flags |= 4;
        } else {
            struct ContactCollider *collider = &effect->collider;
            s32 mask = ~4;
            collider->flags &= mask;
            Collider_SetPosition(collider, (void *)effect->world, 0);
        }
        if (effect->fade <= 63) {
            effect->fade += 2;
            if (effect->fade > 63) {
                effect->sprite.flags &= ~2;
                effect->fade = 64;
            } else {
                effect->sprite.flags |= 2;
            }
        }
        ContactBurst_UpdateContactAnimation(effect);
    } else {
        u32 previous = effect->previousEnabled;
        struct ContactCollider *collider = &effect->collider;
        if (previous) {
            s32 index = 0;
            const u16 *randomTable = (const u16 *)RANDOM_TABLE_ADDR;
            u32 *randomState = &gRandomIndex;
            do {
                u32 sample, frame;
                s32 direction;
                do {
                    *randomState = (*randomState + 1) & 1023;
                    sample = randomTable[*randomState];
                } while (0);
                direction = index << 5;
                sample &= 15;
                direction += sample;
                frame = ContactBurst_RandomParticleFrame();
                DirectionalParticle_Spawn((struct DirectionalParticleSource *)effect, direction,
                                          frame);
                ++index;
            } while (index <= 7);
            Sound_PlaySong(0x3B7);
        }
        effect->sprite.flags |= 1;
        {
            struct ContactCollider *reader = collider;
            u32 flags = reader->flags;
            u32 mask = 4;
            struct ContactCollider *writer;
            mask |= flags;
            writer = collider;
            writer->flags = mask;
        }
    }
    effect->previousEnabled = effect->enabled;
    return 0;
}

s32 ContactBurst_Destroy(struct ContactBurst *effect)
{
    Collider_Remove(&effect->collider);
    Sprite_Remove(&effect->sprite);
    return 0;
}

void ContactBurst_OnContact(struct ContactCollider *, struct ContactCollider *,
                            struct ContactBurst *);

void Collider_SetBodyProperties(void *, u32, u32, u32);

s32 ContactBurst_Initialize(struct ContactBurst *effect, const struct ContactPosition *world)
{
    struct ContactSwarmGraphics *graphics;
    struct TransientEffectSprite *sprite;
    struct ContactCollider *collider;
    struct ContactPackedVector bounds, offset;
    s32 sample;
    s32 status;
    effect->enabled = 0;
    effect->previousEnabled = 0;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    sample = ((const u16 *)RANDOM_TABLE_ADDR)[gRandomIndex];
    effect->contactFrame = Mod(sample >> 8, 3) * 2 + 32;
    effect->fade = 1;
    effect->world = world;
    graphics = &effect->graphics;
    if (!AnimData_Load(graphics, 0x4147)) {
        status = -1;
        goto done;
    }
    sprite = &effect->sprite;
    Sprite_Init(sprite, graphics, 0x211);
    AnimData_SelectPaletteBank(graphics, 0x164);
    sprite->frame = 27;
    sprite->priority = 1;
    bounds.x = 96;
    bounds.y = 128;
    bounds.z = 96;
    offset.x = 0;
    offset.y = 128;
    offset.z = 0;
    collider = &effect->collider;
    Collider_Init(collider, 0, 0x4005, 0, 16, &bounds, &offset);
    Collider_SetCallback(collider, (void *)ContactBurst_OnContact, effect);
    Collider_SetBodyProperties(collider, 0, 0, 0);
    /* The ROM forwards Collider_Refresh's incidental nonnegative return word. */
    status = Collider_Refresh(collider);
done:
    return status;
}

struct ContactBurst *ContactBurst_Create(const struct ContactPosition *world)
{
    struct ContactBurst *effect = Task_Create(8, sizeof(*effect));
    if (effect) {
        Task_SetCallbacks(effect, (void (*)(void *))ContactBurst_Update,
                          (void (*)(void *))ContactBurst_Destroy);
        if (ContactBurst_Initialize(effect, world) < 0) {
            Task_Kill(effect);
            return NULL;
        }
    }
    return effect;
}

void ContactBurst_SetEnabled(struct ContactBurst *effect, u32 enabled)
{
    if (effect)
        effect->enabled = enabled;
}

u32 ContactBurst_GetEnabled(struct ContactBurst *effect)
{
    if (!effect)
        return 0;
    return effect->enabled;
}
