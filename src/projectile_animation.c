#include "global.h"
#include "contact_swarm.h"
#include "player_cue_sequence.h"
#include "engine.h"
#include "monster_particle_random.h"
#include "task4e01_projectile.h"

// Drawable layout shared with ActorDisplay_UpdateForegroundSprite.

struct Drawable {
    struct ContactSwarmSprite sprite;
    struct DisplaySequenceState sequence;
    u32 unknown3C;
    s32 ended;
    u32 mode;
};

static inline u32 StepPlayback(struct Drawable *owner, struct DisplaySequenceState *state)
{
    return PlayerCue_StepSequenceEnded(&owner->sprite, state);
}

void Projectile_SetUpdateCallback(void *, void *);

u32 Projectile_UpdateAnimationTail(u8 *);

s32 Projectile_UpdateAnimation(u8 *p)
{
    u8 *config = p + 0x18;
    void *first = (void *)(p + 0x8c);
    u32 off = 0x180;
    StepPlayback(first, (void *)(p + off));
    if (*(u32 *)p > 4 || (*(u32 *)p > 2 && (*(u16 *)(p + 0x18) & 0x10))) {
        if (*(u16 *)config & 0x10) {
            u16 *counter = (u16 *)(*(u8 **)(config + 0x54) + 0x50);
            *counter = *counter - 1;
            return 2;
        }
        Projectile_SetUpdateCallback(config, Projectile_UpdateAnimationTail);
        *(u32 *)(p + 0x8c) |= 1;
    }
    return 0;
}

u32 Projectile_UpdateAnimationTail(u8 *p)
{
    if (*(s16 *)(p + 0x68) <= 0) {
        return 2;
    }
    *(u32 *)(p + 0x8C) |= 1;
    return 0;
}

struct ProjectileInitView {
    u8 f[0x18];
    u16 fl;
    u8 g[0x8c - 0x1a];
    u32 a;
    u8 h[0xc];
    u16 b;
};

struct ProjectileResetState;
s32 Projectile_UpdateResetDelay(struct ProjectileResetState *);

s32 Projectile_InitResetDelay(u8 *p)
{
    u8 *q;
    Projectile_SetUpdateCallback(p + 0x18, Projectile_UpdateResetDelay);
    if (!(*(u16 *)(p + 0x18) & 0x2000)) {
        q = p + 0x8c;
        *(u16 *)(q + 0x10) = 0x19;
        *(u32 *)q &= ~0xc;
    }
    return 0;
}

struct ProjectileResetState {
    u32 a;
    u32 b;
    u8 f[0x10];
    u16 c;
    u16 d;
};

void Task4E01_BeginImpactAnimation(struct ProjectileResetState *);

s32 Projectile_UpdateResetDelay(struct ProjectileResetState *p)
{
    u32 two = 2;
    p->b |= two;
    if (p->a >= p->d) {
        p->c |= 0x40;
        Task4E01_BeginImpactAnimation(p);
    }
    return 0;
}

void Sound_PlaySong(u32);

void Projectile_InitializeParams(struct Task4E01Params *);

void Projectile_SetFlightParams(struct Task4E01Params *, u32, u32, s32, s32);

void Projectile_SetTypeAndElement(struct Task4E01Params *, u32, u32);

void Projectile_SetSpriteParams(void *, u32, u32, s32);

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
                                       source->sprite[2]);
            Projectile_Spawn(&params, childPosition);
            SceneStimulus_AddUntagged(origin.x, origin.z, 0x400, 0, 1);
        } while (++i < 2);
    }
    return 2;
}

struct PConfig {
    u16 flags;
    u8 pad2[0x38 - 2];
    s16 variant;
    u8 pad3A[0x48 - 0x3A];
    u32 type;
    s32 graphics;
};

void Projectile_InitFromType(struct PConfig *p)
{
    if (p->type != 11) {
        Projectile_SetSpriteParams(p, 0x2112, 0, p->graphics + 0x116);
        if (p->type > 11)
            return;
    }
    switch (p->type) {
        case 1:
            if (p->variant == 1)
                Sound_PlaySong(0x203);
            else
                Sound_PlaySong(0xCD);
            Projectile_SetCollisionSize(p, 0x10, 0x50, 0x10);
            break;
        case 8:
            Sound_PlaySong(0x3CE);
            Projectile_SetCollisionSize(p, 0x10, 0x50, 0x10);
            break;
        case 4:
            Sound_PlaySong(0x3CD);
            Projectile_SetCollisionSize(p, 8, 0x50, 8);
            break;
        case 10:
            Sound_PlaySong(0xCD);
            Projectile_SetCollisionSize(p, 8, 0x50, 8);
            break;
        case 2:
            if (p->variant == 1)
                Sound_PlaySong(0x203);
            else
                Sound_PlaySong(0xCD);
            Projectile_SetCollisionSize(p, 0x10, 0x50, 0x10);
            break;
        case 6:
            Sound_PlaySong(0xCD);
            Projectile_SetCollisionSize(p, 0x20, 0x50, 0x20);
            break;
        case 0:
        case 5:
            Sound_PlaySong(0xCD);
            Projectile_SetCollisionSize(p, 0x10, 0x50, 0x10);
            break;
        case 9:
            Sound_PlaySong(0x164);
            Projectile_SetCollisionSize(p, 8, 0x50, 8);
            break;
        case 3:
            Sound_PlaySong(0x3CF);
            Projectile_SetCollisionSize(p, 8, 0x50, 8);
            break;
        case 7:
            Projectile_SetCollisionSize(p, 8, 0x50, 8);
            break;
        case 11:
            if (!(p->flags & 0x2000)) {
                Sound_PlaySong(0xCD);
                Projectile_SetSpriteParams(p, 0x2112, 0, p->graphics + 0x116);
                Projectile_SetCollisionSize(p, 8, 0x50, 8);
            }
            break;
    }
}
