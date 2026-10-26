#include "global.h"
#include "engine.h"

struct ProjectilePosition {
    s16 x, y, z, unk_06;
};
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
    struct ProjectilePosition velocity;
    struct ProjectileParams params;
    u8 unk_70[0xA8 - 0x70];
    struct ProjectilePosition position;
    u8 unk_B0[0x174 - 0xB0];
    struct ProjectilePosition previousPosition;
};
extern const u16 gUnk_0203B400[];
void Sound_PlaySong(u32);
void Projectile_InitializeParams(struct ProjectileParams *);
void Projectile_SetFlightParams(struct ProjectileParams *, u32, u32, s32, s32);
void Projectile_SetTypeAndElement(struct ProjectileParams *, u32, u32);
void Projectile_SetSpriteParams(struct ProjectileParams *, u32, u32, s32);
s32 Projectile_Spawn(struct ProjectileParams *, struct ProjectilePosition *);
s32 Task4E01_SelectTerrainTurn(struct ProjectilePosition *, struct ProjectilePosition *, s32);
void SceneStimulus_AddUntagged(s32, s32, s32, s32, s32);

static inline s32 RandomValue(void)
{
    const u16 *sample = gUnk_0203B400;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    sample += gRandomIndex;
    return *sample;
}
static inline s32 DirectionSector(s8 angle)
{
    return (((u16)angle + 0x70) >> 5) & 7;
}

static inline void CopyPosition(struct ProjectilePosition *dest, struct ProjectilePosition *src)
{
    *dest = *src;
}
static inline void SubPosition(struct ProjectilePosition *a, struct ProjectilePosition *b)
{
    a->x -= b->x;
    a->y -= b->y;
    a->z -= b->z;
}

#ifdef NONMATCHING
// fuzzy: 81.1%
// scope shape 12 (differences once low registers are set aside)
s32 Projectile_SpawnImpactPair(struct ProjectileEntry *entry)
{
    struct ProjectileParams params;
    struct ProjectilePosition origin;
    struct ProjectileParams *source = &entry->params;
    s32 i;
    if ((source->flags & 0xA0) && !(source->flags & 0x2000)) {
        CopyPosition(&origin, &entry->position);
        SubPosition(&origin, &entry->velocity);
        Sound_PlaySong(0x147);
        for (i = 0; i < 2; i++) {
            u8 first, second;
            Projectile_InitializeParams(&params);
            params.flags |= 0x3000;
            if (DirectionSector(source->direction) & 1) {
                first = (u8)source->direction + (RandomValue() & 63) - 0x80;
                second = (u8)source->direction + (RandomValue() & 63) + 0x40;
            } else {
                s32 turn;
                s32 random = (RandomValue() & 63) - 0x80;
                first = (u8)source->direction + random - 0x20;
                turn = Task4E01_SelectTerrainTurn(&entry->position, &entry->previousPosition,
                                                  DirectionSector(source->direction));
                {
                    s32 random = RandomValue();
                    turn <<= 5;
                    random &= 63;
                    second = random + turn - 0x80;
                }
            }
            Projectile_SetFlightParams(&params, params.flags, 30, (s8)(i == 0 ? first : second), 4);
            params.flags |= source->flags;
            Projectile_SetTypeAndElement(&params, source->type, source->element);
            Projectile_SetSpriteParams(&params, 0x2112, (RandomValue() & 1) == 0 ? 23 : 24,
                                       source->graphics);
            Projectile_Spawn(&params, &entry->position);
            SceneStimulus_AddUntagged(origin.x, origin.z, 0x400, 0, 1);
        }
    }
    return 2;
}
#else
INCLUDE_ASM("asm/nonmatching", Projectile_SpawnImpactPair);
#endif
