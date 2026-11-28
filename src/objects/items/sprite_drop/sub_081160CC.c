#include "global.h"
#include "engine/ram.h"
#include "objects/items/sprite_drop.h"
#include "scene/collision/spatial_index.h"

s32 Terrain_GetCellClearance(s16 *);

// confidence=low: claims a free drop entry at cell (x, y) and returns its index, or -1.
s32 sub_081160CC(s32 x, s32 y, s32 kind)
{
    struct SpriteDropPool *pool;
    s32 i;
    if (!gCompactParticlePool) {
        pool = SpriteDrop_CreateManager();
        if (!pool)
            return -1;
    } else
        pool = gCompactParticlePool;
    for (i = 0; i < SPRITE_DROP_COUNT; i++) {
        if (!pool->items[i].active) {
            struct ContactPosition size, offset, position;
            s32 clearance, duration;

            pool->items[i].x = x << 3;
            pool->items[i].y = y << 3;
            pool->items[i].z = pool->items[i].y - 96;
            clearance = Terrain_GetCellClearance(&pool->items[i].x);
            if (clearance < pool->items[i].y)
                pool->items[i].y = clearance;
            pool->items[i].active = 1;
            pool->items[i].entered = 1;
            pool->items[i].update = SpriteDrop_UpdateEntry;
            pool->items[i].kind = kind;
            if (pool->items[i].kind == 0)
                duration = 600;
            else
                duration = 200;
            size.x = 8;
            size.y = 6;
            size.z = 10;
            offset.x = 0;
            offset.y = -size.y;
            offset.z = -2;
            position.x = pool->items[i].x;
            position.y = pool->items[i].y - (pool->items[i].z + 96);
            position.z = pool->items[i].z;
            SpatialIndex_InitEntry(&pool->items[i].pickup, &position, &size, &offset, 12, 5,
                                   pool->items[i].kind + 3, duration, 12, SpriteDrop_OnExpire,
                                   &pool->items[i]);
            Anim_SetSequence(&pool->items[i].sequence, pool->items[i].sequences, 2,
                             (u8)pool->items[i].kind, 0);
            pool->items[i].sprite.flags |= 1;
            return i;
        }
    }
    return -1;
}
