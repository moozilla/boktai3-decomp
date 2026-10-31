#include "pursuing_actor.h"
#include "engine.h"
#include "ram.h"

struct PursuingActorChaseParameters {
    u16 speed;
    u8 trailIndex, trailPeriod;
    const void *sequenceResource;
    s32 lifetime;
    u16 effectFlags;
};
#define ChaseParameters(p) ((struct PursuingActorChaseParameters *)(p)->unknown1a8)
extern const s16 gUnk_086149C4[];
struct PursuingActorManager *SlashEffect_GetOrCreateManager(void);
struct PursuingActor *SlashEffect_AllocateSlot(struct PursuingActorManager *);
void TwinSpriteEffect_InitGraphics(struct PursuingActor *);
void PursuingActor_InitColliders(struct PursuingActor *, s32, s32, s32, s32, s32, s32, s32);
void TimedEffect_UpdateVisibility(struct PursuingActor *);
void TwinSpriteEffect_EmitTrail(struct PursuingActor *);
void PursuingActor_TurnTowardPlayer(struct PursuingActor *);
void SlashEffect_UpdateColliders(struct PursuingActor *);
void SwooshProjectile_RemoveTrailSprites(struct PursuingActor *);
u8 Sprite_BlinkUntilExpired(void *, u32, s32, s32);
s32 Mod(s32, s32);
s32 Math_Atan2(s32, s32);
static inline s32 Truncate(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}

u8 *TerrainModifiers_FindCellOverride(s32, s32);
static inline s32 CellIndex(struct SceneVec *p)
{
    s32 x = p->x >> 8, z = p->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline u8 *CellData(s32 index)
{
    u8 *p = TerrainModifiers_FindCellOverride(index, 1);
    if (p)
        return p + 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = index * 4 + 12;
        return map->cells + offset;
    }
}
static inline void AddPosition(struct SceneVec *out, struct SceneVec *a, struct SceneVec *b)
{
    {
        u32 v = (u16)a->x;
        out->x = v + (u16)b->x;
    }
    {
        u32 v = (u16)a->y;
        out->y = v + (u16)b->y;
    }
    {
        u32 v = (u16)a->z;
        out->z = v + (u16)b->z;
    }
}
// confidence=low: homing slash travels until a higher terrain cell or its lifetime ends
void PursuingActor_AdvanceChase(struct PursuingActorManager *unused, struct PursuingActor *p)
{
    struct SceneVec next;
    struct SceneVec *velocity;
    struct SceneVec *position;
    s32 angle;
    s32 magnitude;
    s32 cell, x, z;
    {
        u8 *entered = &p->entered;
        if (*entered) {
            p->timer = 0;
            *entered = 0;
        }
    }
    TwinSpriteEffect_EmitTrail(p);
    TimedEffect_UpdateVisibility(p);
    {
        u8 *heading = &p->heading;
        u16 *speed = &ChaseParameters(p)->speed;
        velocity = &p->velocity;
        angle = *heading;
        magnitude = *speed;
    }
    velocity->x = Truncate(gUnk_086149C4[(angle + 64) & 255] * magnitude);
    velocity->y = 0;
    {
        const s16 *sine = gUnk_086149C4;
        u32 index = angle;
        velocity->z = Truncate(sine[index] * magnitude);
    }
    {
        struct SceneVec *out = &next;
        struct SceneVec *current = &p->position;
        struct SceneVec *v = &p->velocity;

        {
            u32 value = (u16)v->x;
            u32 other = (u16)p->position.x;

            next.x = value + other;
        }
        {
            u32 value = (u16)v->y;
            u32 other = (u16)current->y;

            out->y = value + other;
        }
        {
            out->z = (u16)current->z + (u16)v->z;
        }
    }
    x = next.x >> 8;
    z = next.z >> 8;
    position = &p->position;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        cell = 0;
    else
        cell = gTerrainMap->rows[z] + x;
    if ((((*CellData(cell)) & 15) << 8) > p->position.y) {
        p->active = 2;

        p->entered = 1;
    } else {
        AddPosition(&p->position, &p->velocity, &p->position);
        PursuingActor_TurnTowardPlayer(p);
        SlashEffect_UpdateColliders(p);
        p->phase += 8;
        {
            u16 *frame = &p->timer;
            (*frame)++;
            if (!Mod(*frame, 4)) {
                u16 *speed = &ChaseParameters(p)->speed;
                (*speed)++;
            }
        }
        {
            s32 *lifetime = &ChaseParameters(p)->lifetime;
            if (*lifetime > 0)
                (*lifetime)--;
            else {
                p->active = 3;
                p->entered = 1;
            }
        }
    }
}
