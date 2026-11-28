#include "global.h"
#include "engine/ram.h"
#include "scene/collision/packed_contact_vector.h"
#include "objects/items/collectible.h"
#include "scene/collision/spatial_index.h"
s32 Terrain_GetCellClearance(s16 *);

s32 sub_08115AC8(s32 x, s32 y, s32 type, s32 hp)
{
    struct CollectiblePool *pool;
    s32 i;
    if (gActorParticlePool == NULL) {
        pool = Collectible_CreateManager();
        if (pool == NULL)
            return -1;
    } else {
        pool = gActorParticlePool;
    }
    for (i = 0; i < COLLECTIBLE_COUNT; i++) {
        if (!pool->items[i].active) {
            struct ContactPosition pos, size, offset;
            s32 range, seq, flip;
            s32 clearance;
            pool->items[i].x = x << 3;
            pool->items[i].y = y << 3;
            pool->items[i].z = pool->items[i].y - 0x60;
            clearance = Terrain_GetCellClearance(&pool->items[i].x);
            if (clearance < pool->items[i].y)
                pool->items[i].y = clearance;
            pool->items[i].active = 1;
            pool->items[i].entered = 1;
            pool->items[i].update = Collectible_UpdateEntry;
            pool->items[i].health = hp;
            pool->items[i].type = type;
            switch (pool->items[i].type) {
                case 0:
                    size.x = 16;
                    size.y = 8;
                    size.z = 14;
                    offset.x = 0;
                    offset.y = -size.y;
                    offset.z = -10;
                    range = 0x28;
                    seq = 0;
                    flip = 0;
                    break;
                case 2:
                    size.x = 16;
                    size.y = 8;
                    size.z = 14;
                    offset.x = 0;
                    offset.y = -size.y;
                    offset.z = -10;
                    range = 0x50;
                    seq = 0;
                    flip = 1;
                    break;
                case 1:
                    size.x = 8;
                    size.y = 16;
                    size.z = 8;
                    offset.x = 0;
                    offset.y = -size.y;
                    offset.z = 0;
                    range = 0xF0;
                    seq = 3;
                    flip = 0;
                    break;
                default:
                    return -1;
            }
            pos.x = pool->items[i].x;
            pos.y = (s16)(pool->items[i].y - 0x60) - pool->items[i].z;
            pos.z = pool->items[i].z;
            SpatialIndex_InitEntry(&pool->items[i].pickup, &pos, &size, &offset, 12, 6, 2, range,
                                   12, Collectible_OnPickup, &pool->items[i]);
            SpatialIndex_InitEntry(&pool->items[i].body, &pos, &size, &offset, 20, 6, 2, 0, 12,
                                   Collectible_OnContact, &pool->items[i]);
            Anim_SetSequence(&pool->items[i].sequence, pool->items[i].sequences, (u8)seq, (u8)flip,
                             0);
            pool->items[i].sprite.flags |= 1;
            return i;
        }
    }
    return -1;
}
