#include "ui/hud/level_advance_display.h"
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
        s32 i;
        for (i = 0; i < 8; i++) {
            struct LevelAdvanceParticle *particle = &manager->particles[i];
            if (particle->active)
                LevelAdvanceDisplay_UpdateParticle(particle, manager->particleResource);
        }
        if (manager->frame < 64 && !(manager->frame & 3))
            LevelAdvanceDisplay_StartParticle(&manager->particles[(manager->frame >> 2) & 7]);
        {
            u32 next = manager->frame + 1;
            manager->frame = next;
        }
        manager->sprite.position.world = gPlayers[0]->position;
        if (manager->frame < 120) {
            {
                struct LevelAdvanceSprite *sprite = &manager->sprite;
                sprite->frame = 0;
            }
            if (manager->frame < 5) {
                manager->sprite.position.world.y += 250 + manager->frame * 30;
            } else {
                if (manager->frame < 20)
                    LevelAdvanceDisplay_Shake(manager);
                manager->sprite.position.world.y += 400;
            }
        } else if (manager->frame < 123) {
            manager->sprite.position.world.y += 400;
            manager->sprite.frame = 1;
        } else if (manager->frame < 126) {
            manager->sprite.position.world.y += 400;
            manager->sprite.frame = 2;
        } else {
            manager->sprite.flags |= 1;
            manager->active = 0;
            *kindCache = 0;
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
