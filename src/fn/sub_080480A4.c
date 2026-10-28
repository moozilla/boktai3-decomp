#include "contact_burst.h"
#include "projected_display.h"
#include "directional_particles.h"
#include "render.h"
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
