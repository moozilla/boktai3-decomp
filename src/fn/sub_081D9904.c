#include "task4e01_projectile.h"
#include "monster_particle_random.h"

void Projectile_InitializeParams(void *);
void Projectile_SetFlightParams(void *, u32, u32, s32, s32);
void Projectile_SetDamageParams(void *, u32, u32, u32, u32, u32);
void Projectile_SetTypeAndElement(void *, u32, u32);
void Projectile_SetSpriteParams(void *, u32, u32, u32);
s32 Projectile_Spawn(void *, void *);
void Task4E01_BeginImpactAnimation(void *);
s32 Task4E01_SpawnImpactChildren(struct Task4E01Projectile *p)
{
    struct Task4E01Params storage;
    struct Task4E01Params *params;
    struct Task4E01Params *config = &p->params;
    u32 direction = (u8)config->direction;
    s32 speed = config->speed;
    s32 i;
    u32 *randomIndex;
    const u16 *randomTable;
    if (!(config->flags & 0x10)) {
        i = 0;
        do {
            randomIndex = &gRandomIndex;
            params = &storage;
            randomTable = gRandomTable;
            if (config->flags & 0x20) {
                u32 random;
                *randomIndex = (*randomIndex + 1) & 1023;
                PARTICLE_RANDOM_READ_SAMPLE(random, randomTable, *randomIndex);
                {
                    s32 part = random & 63;
                    part -= 32;
                    config->direction = part + (u8)config->direction;
                }
            } else {
                u32 random;
                *randomIndex = (*randomIndex + 1) & 1023;
                PARTICLE_RANDOM_READ_SAMPLE(random, randomTable, *randomIndex);
                config->direction = random;
            }
            Projectile_InitializeParams(&storage);
            if (i == 0)
                params->flags |= 0x110;
            else if (i == 1)
                params->flags = (params->flags & 0xfeef) | 0x210;
            else
                params->flags = (params->flags & 0xfdef) | 0x310;
            {
                s32 half;
                if (speed >= 0)
                    half = speed >> 1;
                else
                    half = -((-speed) >> 1);
                config->speed += half * (i - 1);
            }
            Projectile_SetFlightParams(&storage, params->flags, config->lifetime + 30,
                                       config->direction, config->speed);
            params->flags |= config->flags;
            storage.parent = config;
            Projectile_SetDamageParams(&storage, config->damage[0], config->damage[1],
                                       config->damage[2] >> 1, config->damage[3],
                                       config->damage[4]);
            Projectile_SetCollisionSize(&storage, (s16)config->bounds.x, 80, (s16)config->bounds.z);
            Projectile_SetTypeAndElement(&storage, config->type, config->element);
            Projectile_SetSpriteParams(&storage, config->sprite[0], config->sprite[1],
                                       config->sprite[2]);
            if (Projectile_Spawn(&storage, &p->sprite.position) < 0)
                config->count--;
            config->direction = direction;
            config->speed = speed;
            i++;
        } while (i <= 2);
    }
    PlayerCue_StepSequenceEnded(&p->sprite, &p->sequence);
    Task4E01_BeginImpactAnimation(p);
    return 0;
}
