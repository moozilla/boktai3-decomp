#include "global.h"
#include "hit_spark_random.h"

/* tentative: falling shard with gravity that bursts into debris on landing */

#include "engine.h"
#include "scene_vec.h"

struct SpriteEffectShard {
    u32 flags;
    u8 unk4[0x18];
    struct SceneVec previous;
    u8 unk24[8];
    struct SceneVec position;
    s16 vx, vy, vz, pad;
    u8 collider[0x54];
    u8 shadow[0x40];
    u32 unkD0;
    u16 unkD4;
    u8 active;
    s8 slot;
};

struct SpriteEffectPool {
    u8 unk0[0x34];
    u32 used;
};

struct PackedVec {
    s16 x, y, z, pad;
};

u8 *TerrainModifiers_FindCellOverride(s32, s32);
void Sprite_Remove(void *);
void Shadow_Destroy(void *);
void Collider_SetPosition(void *, void *, u32);
s32 Collider_Refresh(void *);
void sub_08014634(u32, u32, void *, void *, void *, u32, u32);

static inline s32 cellIndex(struct SceneVec *pos)
{
    s32 x = pos->x >> 8, z = pos->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

static inline u32 groundHeight(struct SceneVec *position)
{
    s32 index = cellIndex(position);
    u32 height;
    u8 *cell = TerrainModifiers_FindCellOverride(index, 1);
    u32 kind;
    if (cell)
        cell += 4;
    else
        cell = gTerrainMap->cells + 12 + index * 4;
    kind = *cell >> 4;
    height = (*cell & 15) << 8;
    switch (kind) {
        case 1:
            height -= (u8)position->z;
            break;
        case 2:
            height -= (u8)position->x;
            break;
    }
    return height;
}

/* SpriteEffect_UpdateFallingShard: moves the effect by its velocity under gravity; on reaching the
 * terrain height it plays a sound, releases its pool slot and bursts into debris particles, otherwise
 * it syncs its collider (provisional). */
void SpriteEffect_UpdateFallingShard(struct SpriteEffectPool *manager, struct SpriteEffectShard *p)
{
    struct PackedVec low, high;
    s32 y;
    struct SpriteEffectShard *self = p;
    p->previous = p->position;
    p->flags &= ~1;
    p->position.x += p->vx;
    p->position.y += p->vy;
    p->position.z += p->vz;
    p->vy--;
    y = p->previous.y;
    if (y < groundHeight(&p->previous) || self->previous.y <= 0) {
        Sound_PlaySong(0x1d6);
        manager->used &= ~(1 << p->slot);
        p->active = 0;
        Sprite_Remove(self);
        Shadow_Destroy(p->shadow);
        p->slot = -1;
        p->unkD4 = 0;
        p->unkD0 = 0;
        low.x = -12;
        {
            u32 table = (u32)gRandomTable;
            u32 index = (gRandomIndex + 1) & 1023;
            low.y = HitSpark_Sample(table, index) & 7;
            low.z = -12;
            high.x = 24;
            index = HitSpark_NextIndex(index, 1023);
            gRandomIndex = index;
            high.y = (HitSpark_Sample(table, index) & 7) + 3;
            high.z = 24;
        }
        sub_08014634(6, 3, &p->position, &low, &high, 32, 31);
    } else {
        Collider_SetPosition(p->collider, &p->position, 0);
        Collider_Refresh(p->collider);
    }
}
