#include "level_advance_display.h"
static inline void LevelAdvanceDisplay_Shake(struct LevelAdvanceManager *manager)
{
    const u16 *table = gRandomTable;
    u32 *randomIndex = &gRandomIndex;
    u32 first = (*randomIndex + 1) & 1023;
    u32 second, third;
    {
        s32 value = table[first];
        u32 coordinate = (u16)manager->sprite.position.world.x - 15;
        manager->sprite.position.world.x = coordinate + (value & 31);
    }
    second = (first + 1) & 1023;
    {
        s32 value = table[second];
        u32 coordinate = (u16)manager->sprite.position.world.y - 15;
        manager->sprite.position.world.y = coordinate + (value & 31);
    }
    third = (second + 1) & 1023;
    *randomIndex = third;
    {
        s32 value = table[third];
        u32 coordinate = (u16)manager->sprite.position.world.z - 15;
        manager->sprite.position.world.z = coordinate + (value & 31);
    }
}
void LevelAdvanceDisplay_UpdateAnimation(struct LevelAdvanceManager *manager)
{
    u16 *active = &manager->active;
    u16 *kind;
    u16 *kindCache;
    u32 kindValue;
    if (!*active)
        return;
    kind = &manager->kind;
    kindValue = *kind;
    kindCache = kind;
    if (kindValue == 1) {
        struct LevelAdvanceParticle *particle;
        s32 i;
        u32 zero;
        u16 *frame = &manager->frame;
        particle = manager->particles;
        for (i = 7; i >= 0; i--) {
            if (particle->active)
                LevelAdvanceDisplay_UpdateParticle(particle, manager->particleResource);
            particle++;
        }
        if (*frame < 64 && !(*frame & 3))
            LevelAdvanceDisplay_StartParticle(&manager->particles[(*frame >> 2) & 7]);
        {
            u32 next = *frame + 1;
            zero = 0;
            *frame = next;
        }
        manager->sprite.position.world = gPlayers[0]->position;
        if (*frame < 120) {
            {
                struct LevelAdvanceSprite *sprite = &manager->sprite;
                sprite->frame = zero;
            }
            if (*frame < 5) {
                manager->sprite.position.world.y += 250 + *frame * 30;
            } else {
                if (*frame < 20)
                    LevelAdvanceDisplay_Shake(manager);
                manager->sprite.position.world.y += 400;
            }
        } else if (*frame < 123) {
            manager->sprite.position.world.y += 400;
            manager->sprite.frame = 1;
        } else if (*frame < 126) {
            manager->sprite.position.world.y += 400;
            manager->sprite.frame = 2;
        } else {
            manager->sprite.flags |= 1;
            manager->active = zero;
            *kindCache = zero;
        }
    } else if (kindValue == 2) {
        manager->frame++;
        manager->sprite.position.world = gPlayers[0]->position;
        if (manager->frame < 120) {
            manager->sprite.frame = 3;
            if (manager->frame < 5) {
                manager->sprite.position.world.y += 250 + manager->frame * 30;
            } else {
                if (manager->frame < 20)
                    LevelAdvanceDisplay_Shake(manager);
                manager->sprite.position.world.y += 400;
            }
        } else if (manager->frame < 123) {
            manager->sprite.position.world.y += 400;
            manager->sprite.frame = 4;
        } else if (manager->frame < 126) {
            manager->sprite.position.world.y += 400;
            manager->sprite.frame = 2;
        } else {
            manager->sprite.flags |= 1;
            *active = 0;
            *kind = 0;
        }
    } else {
        manager->sprite.flags |= 1;
        *active = 0;
        *kind = 0;
    }
}
