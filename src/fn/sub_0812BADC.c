#include "pursuing_actor.h"
#include "engine.h"
#include "ram.h"

extern const s16 gUnk_086149C4[];
void TimedEffect_UpdateVisibility(struct PursuingActor *);
void SwooshProjectile_RemoveTrailSprites(struct PursuingActor *);
u8 Sprite_BlinkUntilExpired(void *, u32, s32, s32);
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
    s32 x = p->x >> 8;
    s32 z = p->z >> 8;
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
        u32 offset = (index * 4) + 12;
        return map->cells + offset;
    }
}

static inline void Disable(struct PursuingActorCollider *c)
{
    c->flags |= 4;
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
// confidence=low: contact reaction reverses horizontal velocity, adds upward motion and retires the slash
void PursuingActor_UpdateRecoil(struct PursuingActorManager *pool, struct PursuingActor *p)
{
    struct SceneVec *position;
    struct SceneVec *velocity;
    s32 angle;
    s32 magnitude;
    s32 height;
    if (p->entered) {
        p->timer = 0;
        {
            u32 mask;
            angle = Math_Atan2(p->velocity.x, p->velocity.z) + 128;
            mask = 255;
            angle &= mask;
        }
        velocity = &p->velocity;
        magnitude = 12;
        {
            const s16 *sine = gUnk_086149C4;
            velocity->x = Truncate(sine[(angle + 64) & 255] * magnitude);
        }
        velocity->y = 0;
        {
            const s16 *sine = gUnk_086149C4;
            u32 masked = 255;
            masked &= angle;
            velocity->z = Truncate(sine[masked] * magnitude);
        }
        {
            u32 index = (gRandomIndex + 1) & 1023;
            gRandomIndex = index;
            {
                u32 offset = index * 2;
                const u16 *source;
                source = (const u16 *)RANDOM_TABLE_ADDR;
                {
                    u32 sample = *((const u16 *)(((u32)source) + offset));
                    p->velocity.y = (sample & 15) + 8;
                }
            }
        }
        Disable(&p->primary);
        Disable(&p->secondary);
        Sound_PlaySong(0x1E4);
        p->entered = 0;
    }
    TimedEffect_UpdateVisibility(p);
    AddPosition(&p->position, &p->velocity, &p->position);
    position = &p->position;
    p->velocity.y -= 2;
    height = ((*CellData(CellIndex(position))) & 15) << 8;
    if (p->position.y < height) {
        p->velocity.x = 0;
        p->velocity.y = 0;
        p->velocity.z = 0;
        p->position.y = height;
    }
    if (Sprite_BlinkUntilExpired(p, p->timer, 10, 30)) {
        Collider_Remove(&p->secondary);
        SwooshProjectile_RemoveTrailSprites(p);
        Sprite_Remove(p);
        pool->activeSlots &= ~(1 << p->slot);
        p->slot = -1;
        p->active = 0;
    } else
        p->timer++;
}
