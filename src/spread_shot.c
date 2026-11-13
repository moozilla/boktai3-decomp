#include "spread_shot.h"
#include "global.h"
#include "scene_grid.h"
#include "scene_actor_chase_types.h"
#include "area_effect_link.h"
#include "sprite_notice.h"
#include "scene_actor_reset.h"
#include "ram.h"

/* Bounce-shot terrain probe: may the shot enter the next cell, else slide it along a wall or up
 * a slope toward the player. The match depends on GCSE's expression hash table size (insns at
 * GCSE / 2) and on the pseudo numbers of `cell`, `start` and `preferred`: they order four spilled
 * PRE registers' stack slots. SlopeSurface's u16/s16 round trip and the function-scope `cell`
 * set both; see notes/cloud-08122DF4.md before restructuring. */

u8 *TerrainModifiers_FindCellOverride(s32, s32);

s32 Mod(s32, s32);

static inline struct SceneActorChaseTerrainCell *Properties(s32 index)
{
    u8 *p = TerrainModifiers_FindCellOverride(index, 1);
    if (p)
        return (void *)&((struct SceneActorChaseTerrainOverride *)p)->properties;
    else {
        struct EngineMap *map = gTerrainMap;
        /* Compute the cell offset before loading the map buffer. */
        u32 offset = (u32) & ((struct SceneActorChaseTerrainGrid *)0)->cells[index];
        return (void *)(map->cells + offset);
    }
}

/* Movement-blocking attribute bits of a cell (the halfword after its height byte). */

static inline u16 CellAttributes(s32 index)
{
    return Properties(index)->flags;
}

/* Height of a cell under a position; slopes (shape 1, 2) subtract the low byte of z or x. */

static inline u32 CellHeight(struct SceneActorChaseTerrainCell *properties,
                             struct ContactPosition *position)
{
    u32 shape = (properties->attributes >> 4) & 15;
    u32 height = (properties->attributes & 15) << 8;
    if (shape == 1)
        goto slopeZ;
    if (shape == 2)
        goto slopeX;
    goto done;
slopeZ:
    height -= *(u8 *)&position->z;
    goto done;
slopeX:
    height -= *(u8 *)&position->x;
done:
    return height;
}

/* Surface height a shot riding a slope cell takes on (its y). */

static inline u16 SlopeSurface(struct SceneActorChaseTerrainCell *properties,
                               struct ContactPosition *position)
{
    s16 height = CellHeight(properties, position);
    return height;
}

/* Can the shot enter `next` from `current` moving in quadrant `direction`? */

static inline u8 CanEnter(struct ContactPosition *position, u32 current, u32 next, s32 angle)
{
    switch (((angle + 0x20) >> 6) & 3) {
        case 0:
            if (Properties(current)->attributes >> 4 == 2)
                return 1;
            if (Properties(next)->attributes >> 4 == 2)
                return 1;
            if (CellAttributes(next) & 0xC3)
                return 0;
            if (CellHeight(Properties(next), position) == position->y)
                return 1;
            return 0;
        case 1:
            if (Properties(current)->attributes >> 4 == 1)
                return 1;
            if (Properties(next)->attributes >> 4 == 1)
                return 1;
            if (CellAttributes(next) & 0xC3)
                return 0;
            if (CellHeight(Properties(next), position) == position->y)
                return 1;
            return 0;
        case 2:
            if (Properties(current)->attributes >> 4 == 2)
                return 1;
            if (Properties(next)->attributes >> 4 == 2)
                return 1;
            if (CellAttributes(next) & 0xC3)
                return 0;
            if (CellHeight(Properties(next), position) == position->y)
                return 1;
            return 0;
        case 3:
            if (Properties(current)->attributes >> 4 == 1)
                return 1;
            if (Properties(next)->attributes >> 4 == 1)
                return 1;
            if (CellAttributes(next) & 0xC3)
                return 0;
            if (CellHeight(Properties(next), position) == position->y)
                return 1;
            return 0;
    }
    return 0;
}

/* Walk from `cell` towards `target` along the row, then the column, over cells of the same
 * height; a slope cell ends the walk (the shot rides it). Returns 1 if the walk reaches. */

static inline u8 Seek(struct ContactPosition *position, s32 cell, s32 target)
{
    s32 level = position->y >> 8;
    s32 width = SceneGrid_Width();
    s32 total = width * ((struct SceneActorChaseTerrainGrid *)gTerrainMap->cells)->height;
    s32 i, column;
    if (cell < target) {
        i = cell + 1;
        column = Mod(i, width);
        while (column < SceneGrid_Width()) {
            if ((Properties(i)->attributes & 15) != level)
                break;
            if (CellAttributes(i) & 0xC3)
                break;
            if (Properties(i)->attributes >> 4 == 2) {
                position->y = SlopeSurface(Properties(i), position);
                return 1;
            }
            if (i == target)
                return 1;
            i++;
            column++;
        }
        for (i = cell + SceneGrid_Width(); total > i; i += SceneGrid_Width()) {
            if ((Properties(i)->attributes & 15) != level)
                break;
            if (CellAttributes(i) & 0xC3)
                break;
            if (Properties(i)->attributes >> 4 == 1) {
                position->y = SlopeSurface(Properties(i), position);
                return 1;
            }
            if (i == target)
                return 1;
        }
        return 0;
    } else {
        i = cell - 1;
        column = Mod(i, width);
        while (column >= 0) {
            if ((Properties(i)->attributes & 15) != level)
                break;
            if (CellAttributes(i) & 0xC3)
                break;
            if (Properties(i)->attributes >> 4 == 2) {
                position->y = SlopeSurface(Properties(i), position);
                return 1;
            }
            if (i == target)
                return 1;
            i--;
            column--;
        }
        for (i = cell - SceneGrid_Width(); i >= 0; i -= SceneGrid_Width()) {
            if ((Properties(i)->attributes & 15) != level)
                break;
            if (CellAttributes(i) & 0xC3)
                break;
            if (Properties(i)->attributes >> 4 == 1) {
                position->y = SlopeSurface(Properties(i), position);
                return 1;
            }
            if (i == target)
                return 1;
        }
    }
    return 0;
}

static inline void Delta(struct ContactPosition *out, struct ContactPosition *a,
                         struct ContactPosition *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}

u8 SpreadShot_ResolveTerrain(struct SpreadShot *shot, s32 angle, u32 next)
{
    struct ContactPosition *position;
    u16 *currentCell;
    s32 quadrant;
    s32 redirectDirection;
    s32 speed;
    s32 cell;
    if (shot->kind) {
        if (CanEnter(&shot->position, shot->cell, next, angle))
            return 0;
        currentCell = &shot->cell;
        position = &shot->position;
        quadrant = angle + 0x20;
        if (gEngineFlags & 0x800) {
            s32 target;
            cell = *currentCell;
            switch ((quadrant >> 6) & 3) {
                case 0:
                case 2:
                    if (Seek(position, cell, cell - SceneGrid_Width())) {
                        redirectDirection = 3;
                        break;
                    }
                    if (!Seek(position, cell, cell + SceneGrid_Width()))
                        goto stop;
                    redirectDirection = 1;
                    break;
                case 1:
                case 3:
                    target = cell + 1;
                    if (Seek(position, cell, target)) {
                        redirectDirection = 0;
                        break;
                    }
                    target = cell - 1;
                    {
                        u8 found = Seek(position, cell, target);
                        redirectDirection = 2;
                        if (found)
                            goto check;
                        goto stop;
                    }

                default:
                    goto stop;
            }
        } else {
            u8 found[4];
            struct ContactPosition delta;
            s32 start = *currentCell;
            s32 direction = (quadrant >> 6) & 3;
            s32 preferred;
            s32 target;
            struct AreaEffectPlayer *player = gPlayers[0];
            if (!player)
                preferred = 0;
            else {
                Delta(&delta, (struct ContactPosition *)&player->position, position);
                preferred = ((Math_Atan2(delta.x, delta.z) + 0x20) & 0xFF) >> 6;
            }
            Memory_Clear(found, 4);
            switch (direction) {
                case 0:
                case 2:
                    if (Seek(position, start, start - SceneGrid_Width()))
                        found[3] = 1;
                    if (Seek(position, start, start + SceneGrid_Width()))
                        found[1] = 1;
                    break;
                case 1:
                case 3:
                    target = start + 1;
                    if (Seek(position, start, target))
                        found[0] = 1;
                    target = start - 1;
                    if (Seek(position, start, target))
                        found[2] = 1;
                    break;
            }
            if (found[preferred] == 1)
                redirectDirection = preferred;
            else {
                s32 i;
                for (i = 0; i <= 3; i++) {
                    if (found[i]) {
                        redirectDirection = i;
                        goto check;
                    }
                }
                redirectDirection = -1;
            }
        }
    check:
        if (redirectDirection < 0)
            goto stop;
        {

            s32 speedX, speedZ;
            speedX = shot->velocity.x;
            if (speedX < 0)
                speedX = -speedX;
            speedZ = shot->velocity.z;
            if (speedZ < 0)
                speedZ = -speedZ;
            speed = speedZ;
            if (speed < speedX)
                speed = speedX;
            switch (redirectDirection) {
                case 0:
                    shot->velocity.x = speed;
                    shot->velocity.z = 0;
                    break;
                case 1:
                    shot->velocity.x = 0;
                    shot->velocity.z = speed;
                    break;
                case 2:
                    shot->velocity.x = -speed;
                    shot->velocity.z = 0;
                    break;
                case 3:
                    shot->velocity.x = 0;
                    shot->velocity.z = -speed;
                    break;
            }
            return 1;
        }
    } else if (!CanEnter(&shot->position, shot->cell, next, angle)) {
    stop:
        shot->state = 2;
        return 1;
    }
    return 0;
}

/* SceneTerrainProbe_NoopCollisionHook: Empty hook beside terrain collision checks; subsystem and hook role are tentative */

void SceneTerrainProbe_NoopCollisionHook(void)
{
}

s32 Div(s32, s32);

/* These callers retain the unused third argument passed by the original notice API. */

s32 SpriteNotice_CreateBetweenPositions(struct ContactPosition *start, struct ContactPosition *end,
                                        u32 kind)
{
    struct ContactPosition values;
    struct ContactPosition *delta = &values;
    values.x = end->x - start->x;
    delta->y = end->y - start->y;
    delta->z = end->z - start->z;
    if (values.x * values.x + delta->z * delta->z > 0x3FF) {
        values.x = start->x + Div(values.x, 2);
        delta->y = start->y;
        delta->z = start->z + Div(delta->z, 2);
        if (((s32(*)(const struct ContactPosition *, u32, u32))SpriteNotice_Create)(delta, 2,
                                                                                    kind) < 0)
            goto fail;
    }
    return 1;
fail:
    return 0;
}

/* SceneTerrainProbe_NoopTrailHook: Empty hook following a helper that emits an effect between positions; role is tentative */

void SceneTerrainProbe_NoopTrailHook(void)
{
}

/* Spread-shot projectile step: advance, collide, spawn impact notices */

/* Keep the cell lookup and height calculation together here: sharing CellHeight
 * changes the step's register allocation and removes an original instruction. */

void BurstSpark_Create(struct ContactPosition *, u32, u32, u32, u32, u32, u32);

static inline u32 CellIndex(struct ContactPosition *position)
{
    s32 x = position->x >> 8, z = position->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}

static inline u32 SpreadShot_GetCellHeight(struct ContactPosition *position, u32 cell)
{
    u32 shape, height;
    struct SceneActorChaseTerrainCell *data = (void *)TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data = (void *)&((struct SceneActorChaseTerrainOverride *)data)->properties;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = (u32) & ((struct SceneActorChaseTerrainGrid *)0)->cells[cell];
        data = (void *)(map->cells + offset);
    }
    shape = data->attributes >> 4;
    height = (data->attributes & 15) << 8;
    if (shape == 1)
        goto slopeZ;
    if (shape == 2)
        goto slopeX;
    goto done;
slopeZ:
    height -= *(u8 *)&position->z;
    goto done;
slopeX:
    height -= *(u8 *)&position->x;
done:
    return height;
}

static inline u32 Random(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return *(const u16 *)(index * 2 + RANDOM_TABLE_ADDR);
}

void SpreadShot_Step(u32 unused, struct SpreadShot *shot)
{
    u16 *timer;
    s32 angle;
    u32 cell;
    struct ContactPosition noticePosition;
    struct ContactPosition previousPosition;
    if (shot->reset)
        shot->reset = 0;
    {
        u16 remaining = shot->timer;
        timer = &shot->timer;
        if (remaining) {
            *timer = remaining - 1;
            return;
        }
    }

    {
        u16 *currentCell;
        s16 *velocityX;
        shot->cell = CellIndex(&shot->position);
        currentCell = &shot->cell;
        velocityX = &shot->velocity.x;
        angle = Math_Atan2(*velocityX, shot->velocity.z);
        previousPosition = shot->position;
        shot->position.x += *velocityX;
        shot->position.y = SpreadShot_GetCellHeight(&shot->position, *currentCell);
        shot->position.z += shot->velocity.z;
        cell = CellIndex(&shot->position);
        Collider_SetPosition(&shot->collider, &shot->position, 0);
        Collider_Refresh(&shot->collider);
        if (SpreadShot_ResolveTerrain(shot, angle, cell))
            shot->position = previousPosition;
        else
            shot->position.y = SpreadShot_GetCellHeight(&shot->position, cell);
        shot->body.position = shot->position;
        noticePosition.x = shot->position.x;
        noticePosition.y = shot->position.y;
        noticePosition.z = shot->position.z;
    }
    switch (angle >> 6) {
        case 0:
        case 2:
            noticePosition.z += Mod(Random(), 120) - 60;
            break;
        case 1:
        case 3:
            noticePosition.x += Mod(Random(), 120) - 60;
            break;
    }
    if (shot->kind == 2) {
        s32 i;
        for (i = 0; i <= 3; i++)
            BurstSpark_Create(&noticePosition, 1, 0, i << 6, 0x54, 1, 0);
        for (i = 0; i <= 1; i++) {
            BurstSpark_Create(&noticePosition, 1, 8, i << 6, 0x22, 1, 0);
            BurstSpark_Create(&noticePosition, 1, 14, i << 6, 0x40, 1, 0);
        }
        *timer = 8;
    } else {
        if (((s32(*)(const struct ContactPosition *, u32, u32))SpriteNotice_Create)(
                &noticePosition, 2, shot->kind) < 0)
            goto done;
        if (!(u8)SpriteNotice_CreateBetweenPositions(&previousPosition, &noticePosition,
                                                     shot->kind))
            goto done;
        *timer = (Random() & 7) + 3;
    }
    if (shot->hits) {
        shot->hits--;
        if (shot->kind != 2)
            Sound_PlaySong(0x1E2);
        return;
    }
done:
    shot->state = 2;
}

void TerrainOverlay_QueueRemoveActor(void *);

void SceneBody_Unregister(void *);

void SceneTerrainProbe_DestroyEntry(struct PairSlotPool *pool, struct SpreadShot *shot)
{
    u8 *slotIndex;
    s32 index;
    shot->state = 0;
    slotIndex = (u8 *)&shot->index;
    index = *(s8 *)slotIndex;
    pool->allocated &= ~(1 << index);
    TerrainOverlay_QueueRemoveActor(shot);
    SceneBody_Unregister(shot);
    Memory_Clear(shot, sizeof(*shot));
    *slotIndex = 0xff;
}

static inline u8 PairSlotPool_IsAllocated(struct PairSlotPool *pool, s32 index)
{
    if (pool->allocated & (1 << index))
        return TRUE;
    return FALSE;
}

struct SpreadShot *PairSlotPool_Allocate(struct PairSlotPool *pool)
{
    s32 index;
    for (index = 0; index < 2; index++) {
        if (!PairSlotPool_IsAllocated(pool, index)) {
            struct SpreadShot *entry = &pool->entries[index];
            entry->index = index;
            pool->allocated |= 1 << index;
            return entry;
        }
    }
    return NULL;
}

void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);

void SceneBody_Init(void *, u32, void *, u32, u32, void *);

void SceneBody_SetSprite(void *, void *);

void TerrainOverlay_QueueAddActor(void *);

static inline void SpreadShot_InitCollider(void *collider, u32 id, u32 flags, u32 a, u32 b,
                                           void *size, void *offset)
{
    Collider_Init(collider, id, flags, a, b, size, offset);
}

static inline void SpreadShot_SetAttackProperties(void *collider, u32 a, u32 b, u32 flags,
                                                  u32 properties, u32 c)
{
    Collider_SetAttackProperties(collider, a, b, flags, properties, c);
}

s32 SpreadShot_Create(struct ContactPosition *position, struct ContactPosition *velocity, u32 hits,
                      u32 kind, u32 attackA, u32 attackB, u32 attackC)
{
    struct SpreadShot *shot;
    struct PairSlotPool *pool;
    struct ContactCollider *collider;
    struct ContactPosition size, offset;

    if (!position)
        return -1;
    pool = gPairSlotPool;
    if (!pool) {
        pool = SceneTerrainProbe_EnsurePool();
        if (!pool)
            return -1;
    }
    shot = PairSlotPool_Allocate(pool);
    if (!shot)
        return -1;
    shot->position = *position;
    shot->velocity = *velocity;
    shot->hits = hits;
    shot->kind = kind;
    shot->timer = 0;
    shot->state = 1;
    collider = &shot->collider;
    size.x = 0x40;
    size.y = 0x40;
    size.z = 0x40;
    offset.x = 0;
    offset.y = 0x40;
    offset.z = 0;
    if (shot->kind == 2) {
        SpreadShot_InitCollider(collider, 0, 0x2001, 0, 0, &size, &offset);
        SpreadShot_SetAttackProperties(collider, attackA, attackB, 0x200, 0x240004, attackC);
    } else {
        SpreadShot_InitCollider(collider, 0, 0x2001, 0, 0x10, &size, &offset);
        SpreadShot_SetAttackProperties(collider, attackA, attackB, 0x10, 0, attackC);
    }
    collider->collisionId = shot->body.id;
    Collider_SetPosition(collider, &shot->position, 0);
    Collider_SetCallback(collider, SceneTerrainProbe_NoopCollisionHook, shot);
    SceneBody_Init(shot, shot->index + 0x309, &shot->position, 0, 7, shot);
    SceneBody_SetSprite(shot, &shot->sprite);
    TerrainOverlay_QueueAddActor(shot);
    return 0;
}

extern void *gUnk_08610734[];

s32 PairSlotPool_Update(struct PairSlotPool *pool)
{
    s32 index;
    if (!pool->allocated)
        return 0;
    for (index = 0; index < 2; index++) {
        if (PairSlotPool_IsAllocated(pool, index)) {
            struct SpreadShot *entry = &pool->entries[index];
            sub_08249240(pool, entry, gUnk_08610734[entry->state]);
        }
    }
    return 0;
}

void SceneTerrainProbe_OnPoolDestroy(void)
{
    gPairSlotPool = 0;
}

s32 SceneTerrainProbe_InitPool(struct PairSlotPool *pool)
{
    gPairSlotPool = pool;
    return 0;
}

struct PairSlotPool *SceneTerrainProbe_EnsurePool(void)
{
    struct PairSlotPool *pool;
    if (gPairSlotPool != 0)
        return gPairSlotPool;
    pool = Task_Create(10, sizeof(*pool));
    if (pool != 0) {
        Task_SetCallbacks(pool, (void (*)(void *))PairSlotPool_Update,
                          (void (*)(void *))SceneTerrainProbe_OnPoolDestroy);
        if (SceneTerrainProbe_InitPool(pool) < 0) {
            Task_Kill(pool);
            return 0;
        }
    }
    return pool;
}

void SceneTerrainProbe_ResetPool(void)
{
    gPairSlotPool = 0;
}
