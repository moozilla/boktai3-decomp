// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "engine.h"
#include "monster_particle_random.h"
#include "contact_position.h"

struct ProjectileParams {
    u16 flags, lifetime;
    u8 unk_04[2];
    s8 direction;
    u8 unk_07;
    s32 speed;
    u8 unk_0C[0x44 - 0xC];
    s32 graphics;
    u32 type, element;
    u8 unk_50[8];
};
struct ProjectileEntry {
    u8 unk_00[0x10];
    struct ContactPosition velocity;
    struct ProjectileParams params;
    u8 unk_70[0xA8 - 0x70];
    struct ContactPosition position;
    u8 unk_B0[0x174 - 0xB0];
    struct ContactPosition previousPosition;
};
void Sound_PlaySong(u32);
void Projectile_InitializeParams(struct ProjectileParams *);
void Projectile_SetFlightParams(struct ProjectileParams *, u32, u32, s32, s32);
void Projectile_SetTypeAndElement(struct ProjectileParams *, u32, u32);
void Projectile_SetSpriteParams(struct ProjectileParams *, u32, u32, s32);
s32 Projectile_Spawn(struct ProjectileParams *, struct ContactPosition *);
s32 Task4E01_SelectTerrainTurn(struct ContactPosition *, struct ContactPosition *, s32);
void SceneStimulus_AddUntagged(s32, s32, s32, s32, s32);

#define INIT_IMPACT_RANDOM(cursor, ringMask, samples)                                              \
    do {                                                                                           \
        (cursor) = &gRandomIndex;                                                                  \
        (ringMask) = 1023;                                                                         \
        (samples) = gRandomTable;                                                                  \
    } while (0)

static inline s32 ReadSample(const u16 *table, u32 *rng, u32 mask)
{
    u32 sample;
    *rng = (*rng + 1) & mask;
    PARTICLE_RANDOM_READ_SAMPLE(sample, table, *rng);
    return sample;
}
static inline s32 DirectionSector(s8 angle, u32 sectorMask)
{
    return (((u16)angle + 0x70) >> 5) & sectorMask;
}

static inline void CopyPosition(struct ContactPosition *dest, struct ContactPosition *src)
{
    *dest = *src;
}
static inline void SubPosition(struct ContactPosition *a, struct ContactPosition *b)
{
    a->x -= b->x;
    a->y -= b->y;
    a->z -= b->z;
}

s32 Projectile_SpawnImpactPair(struct ProjectileEntry *entry)
{
    struct ProjectileParams params;
    struct ContactPosition origin;
    struct ProjectileParams *source = &entry->params;
    s32 i;
    u16 jitterMask;
    u32 sectorMask;
    struct ContactPosition *childPosition;
    struct ProjectileParams *out;
    u32 *rng;
    u32 mask;
    const u16 *table;

    if ((source->flags & 0xA0) && !(source->flags & 0x2000)) {
        CopyPosition(&origin, &entry->position);
        SubPosition(&origin, &entry->velocity);
        Sound_PlaySong(0x147);

        i = 0;
        childPosition = &entry->position;
        out = &params;
        INIT_IMPACT_RANDOM(rng, mask, table);
        jitterMask = 63;
        sectorMask = 7;

        for (; i < 2; i++) {
            u8 first, second;
            Projectile_InitializeParams(&params);
            out->flags |= 0x3000;
            if (DirectionSector(source->direction, sectorMask) & 1) {
                first = (u8)source->direction + (ReadSample(table, rng, mask) & jitterMask) - 0x80;
                second = (u8)source->direction + (ReadSample(table, rng, mask) & jitterMask) + 0x40;
            } else {
                s32 turn;
                s32 random = (ReadSample(table, rng, mask) & jitterMask) - 0x80;
                first = (u8)source->direction + random - 0x20;
                turn = Task4E01_SelectTerrainTurn(childPosition, &entry->previousPosition,
                                                  DirectionSector(source->direction, sectorMask));
                {
                    s32 random = ReadSample(table, rng, mask);
                    turn <<= 5;
                    random &= jitterMask;
                    second = random + turn - 0x80;
                }
            }
            Projectile_SetFlightParams(&params, out->flags, 30, (s8)(i == 0 ? first : second), 4);
            out->flags |= source->flags;
            Projectile_SetTypeAndElement(&params, source->type, source->element);
            Projectile_SetSpriteParams(&params, 0x2112,
                                       (ReadSample(table, rng, mask) & 1) == 0 ? 23 : 24,
                                       source->graphics);
            Projectile_Spawn(&params, childPosition);
            SceneStimulus_AddUntagged(origin.x, origin.z, 0x400, 0, 1);
        }
    }
    return 2;
}
