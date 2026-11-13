#include "projectile_impact.h"
#include "monster_particle_random.h"

// Drawable layout shared with ActorDisplay_UpdateForegroundSprite.

s32 Projectile_UpdateAnimation(struct Task4E01Projectile *projectile)
{
    struct Task4E01Params *config = &projectile->params;
    struct ContactSwarmSprite *first = &projectile->sprite;
    PlayerCue_StepSequenceEnded(first, &projectile->runtime.sequence);
    if (projectile->frame > 4 || (projectile->frame > 2 && (projectile->params.flags & 0x10))) {
        if (config->flags & 0x10) {
            u16 *counter = &config->parent->count;
            *counter = *counter - 1;
            return 2;
        }
        Projectile_SetUpdateCallback(config, Projectile_UpdateAnimationTail);
        projectile->sprite.flags |= 1;
    }
    return 0;
}

u32 Projectile_UpdateAnimationTail(struct Task4E01Projectile *projectile)
{
    if ((s16)projectile->params.count <= 0) {
        return 2;
    }
    projectile->sprite.flags |= 1;
    return 0;
}

s32 Projectile_InitResetDelay(struct Task4E01Projectile *projectile)
{
    struct ContactSwarmSprite *sprite;
    Projectile_SetUpdateCallback(&projectile->params, Projectile_UpdateResetDelay);
    if (!(projectile->params.flags & 0x2000)) {
        sprite = &projectile->sprite;
        sprite->frame = 0x19;
        sprite->flags &= ~0xc;
    }
    return 0;
}

s32 Projectile_UpdateResetDelay(struct Task4E01Projectile *projectile)
{
    u32 two = 2;
    projectile->flags |= two;
    if (projectile->frame >= projectile->params.lifetime) {
        projectile->params.flags |= 0x40;
        Task4E01_BeginImpactAnimation(projectile);
    }
    return 0;
}

void Sound_PlaySong(u32);

void Projectile_InitializeParams(struct Task4E01Params *);

void Projectile_SetFlightParams(struct Task4E01Params *, u32, u32, s32, s32);

void Projectile_SetTypeAndElement(struct Task4E01Params *, u32, u32);

void Projectile_SetSpriteParams(struct Task4E01Params *, u32, u32, s32);

s32 Projectile_Spawn(struct Task4E01Params *, struct ContactPosition *);

s32 Task4E01_SelectTerrainTurn(struct ContactPosition *, struct ContactPosition *, s32);

void SceneStimulus_AddUntagged(s32, s32, s32, s32, s32);

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

s32 Projectile_SpawnImpactPair(struct Task4E01Projectile *entry)
{
    struct Task4E01Params params;
    struct ContactPosition origin;
    struct Task4E01Params *source = &entry->params;
    s32 i;
    u16 jitterMask;
    u32 sectorMask;
    struct ContactPosition *childPosition;
    struct Task4E01Params *out;
    u32 *rng;
    u32 mask;
    const u16 *table;

    if ((source->flags & 0xA0) && !(source->flags & 0x2000)) {
        CopyPosition(&origin, &entry->sprite.position);
        SubPosition(&origin, &entry->velocity);
        Sound_PlaySong(0x147);

        i = 0;
        childPosition = &entry->sprite.position;
        out = &params;
        jitterMask = 63;
        sectorMask = 7;
        do {
            u8 first, second;
            rng = &gRandomIndex;
            mask = 1023;
            table = gRandomTable;
            Projectile_InitializeParams(&params);
            out->flags |= 0x3000;
            if (DirectionSector(source->direction, sectorMask) & 1) {
                first = (u8)source->direction + (ReadSample(table, rng, mask) & jitterMask) - 0x80;
                second = (u8)source->direction + (ReadSample(table, rng, mask) & jitterMask) + 0x40;
            } else {
                s32 turn;
                s32 random = (ReadSample(table, rng, mask) & jitterMask) - 0x80;
                first = (u8)source->direction + random - 0x20;
                turn = Task4E01_SelectTerrainTurn(childPosition, &entry->runtime.previousPosition,
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
                                       source->sprite[2]);
            Projectile_Spawn(&params, childPosition);
            SceneStimulus_AddUntagged(origin.x, origin.z, 0x400, 0, 1);
        } while (++i < 2);
    }
    return 2;
}


void Projectile_InitFromType(struct Task4E01Params *params)
{
    if (params->type != 11) {
        Projectile_SetSpriteParams(params, 0x2112, 0, params->element + 0x116);
        if (params->type > 11)
            return;
    }
    switch (params->type) {
        case 1:
            if (params->variant == 1)
                Sound_PlaySong(0x203);
            else
                Sound_PlaySong(0xCD);
            Projectile_SetCollisionSize(params, 0x10, 0x50, 0x10);
            break;
        case 8:
            Sound_PlaySong(0x3CE);
            Projectile_SetCollisionSize(params, 0x10, 0x50, 0x10);
            break;
        case 4:
            Sound_PlaySong(0x3CD);
            Projectile_SetCollisionSize(params, 8, 0x50, 8);
            break;
        case 10:
            Sound_PlaySong(0xCD);
            Projectile_SetCollisionSize(params, 8, 0x50, 8);
            break;
        case 2:
            if (params->variant == 1)
                Sound_PlaySong(0x203);
            else
                Sound_PlaySong(0xCD);
            Projectile_SetCollisionSize(params, 0x10, 0x50, 0x10);
            break;
        case 6:
            Sound_PlaySong(0xCD);
            Projectile_SetCollisionSize(params, 0x20, 0x50, 0x20);
            break;
        case 0:
        case 5:
            Sound_PlaySong(0xCD);
            Projectile_SetCollisionSize(params, 0x10, 0x50, 0x10);
            break;
        case 9:
            Sound_PlaySong(0x164);
            Projectile_SetCollisionSize(params, 8, 0x50, 8);
            break;
        case 3:
            Sound_PlaySong(0x3CF);
            Projectile_SetCollisionSize(params, 8, 0x50, 8);
            break;
        case 7:
            Projectile_SetCollisionSize(params, 8, 0x50, 8);
            break;
        case 11:
            if (!(params->flags & 0x2000)) {
                Sound_PlaySong(0xCD);
                Projectile_SetSpriteParams(params, 0x2112, 0, params->element + 0x116);
                Projectile_SetCollisionSize(params, 8, 0x50, 8);
            }
            break;
    }
}
