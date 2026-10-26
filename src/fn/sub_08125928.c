#include "global.h"
#include "ram.h"
struct ContactPosition {
    s16 x, y, z, pad;
} __attribute__((aligned(4)));
struct SpreadShotCollider {
    u32 next;
    u16 id;
    u8 unk_06[0x4e];
};

struct SpreadShot {
    u32 id;
    u8 unk_04[0x44];
    u8 sprite[0x2c];
    struct ContactPosition position;
    struct ContactPosition velocity;
    struct SpreadShotCollider collider;
    u16 hits;
    u8 kind;
    u8 reset;
    u16 cell;
    s8 index;
    u8 state;
    u16 timer;
};
struct SpreadShotExtent {
    u32 x : 16;
    u32 y : 16;
    u32 z : 16;
    u32 : 16;
};
struct PairSlotPool;
struct PairSlotPool *SceneTerrainProbe_EnsurePool(void);
struct SpreadShot *PairSlotPool_Allocate(struct PairSlotPool *);
void Collider_Init(void *, u16, u32, u32, u32, void *, void *);
void Collider_SetCallback(void *, void *, void *);
void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);
void Collider_SetPosition(void *, void *, u32);
void sub_08125498(void);
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
    struct SpreadShotCollider *collider;
    struct SpreadShotExtent size, offset;

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
    collider->id = shot->id;
    Collider_SetPosition(collider, &shot->position, 0);
    Collider_SetCallback(collider, sub_08125498, shot);
    SceneBody_Init(shot, shot->index + 0x309, &shot->position, 0, 7, shot);
    SceneBody_SetSprite(shot, shot->sprite);
    TerrainOverlay_QueueAddActor(shot);
    return 0;
}
