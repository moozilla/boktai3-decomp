#include "engine/shared/contact_actor.h"
#include "engine/shared/monster_particle_random.h"
#include "engine/terrain/terrain_sampling.h"
#include "engine/shared/scene_actor_part_burst.h"
void ContactActor_SetDirectionalSequence(struct ContactActorPool *, struct ContactActor *, u32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
static inline s32 truncateShift12(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
static inline u32 nextRandomFrom(const u16 *table, u32 *cursor)
{
    u32 index;
    do {
        index = (*cursor + 1) & 1023;
        *cursor = index;
    } while (0);
    return table[index];
}
static inline void addPosition(struct ContactPosition *out, const struct ContactPosition *delta,
                               const struct ContactPosition *base)
{
    out->x = base->x + delta->x;
    out->y = base->y + delta->y;
    out->z = base->z + delta->z;
}

void ContactActor_UpdateRetreat(struct ContactActorPool *pool, struct ContactActor *slot, s32 index)
{
    struct ContactPosition delta;
    struct ContactPosition *position;
    struct ContactPosition *destination;
    if (slot->enter) {
        struct ContactPosition *center;
        struct ContactPosition *entryDestination;
        u32 origin;
        const u16 *table;
        const u16 *earlyTable;
        u32 angle;
        union {
            u32 mask;
            u32 *cursor;
        } random;
        u32 radiusBits;
        u32 radius;
        s32 jitter;
        struct ContactPosition *write;
        slot->enter = 0;
        {
            struct ContactCollider *collider = &slot->healthCollider;
            collider->flags |= 4;
        }
        center = &pool->retreatCenter;
        pool->retreatCenter.y = Terrain_SampleHeight(center);
        PARTICLE_RANDOM_CAPTURE_ORIGIN(origin);
        earlyTable = (const u16 *)0x0203B400;
        angle = SceneActorEffects_NextSample(earlyTable);
        do {
            random.mask = 255;
        } while (0);
        angle &= random.mask;
        radiusBits = SceneActorEffects_NextSample(earlyTable);
        do {
            radiusBits &= 127;
        } while (0);
        write = &delta;
        radius = radiusBits + 16;
        {
            s32 product = gUnk_086149C4[(angle + 64) & random.mask] * radius;
            random.cursor = &gRandomIndex;
            write->x = truncateShift12(product) | (origin & 3);
        }
        write->y = 0;
        write->z = truncateShift12(gUnk_086149C4[angle] * radius);
        entryDestination = &slot->destination;
        addPosition(entryDestination, &delta, &pool->retreatCenter);
        table = gRandomTable;
        slot->targetHeight = (nextRandomFrom(table, random.cursor) & 511) + slot->desiredHeight;
        slot->direction = Math_Atan2(slot->destination.x - slot->position.x,
                                     entryDestination->z - slot->position.z);
        jitter = (nextRandomFrom(table, random.cursor) & 15) - 8;
        slot->direction += jitter;
        slot->steeringTimer = (nextRandomFrom(table, random.cursor) & 7) + 8;
        slot->speed = 16;
    }
    if (!slot->steeringTimer) {
        const u16 *table;
        s32 jitter;
        struct ContactPosition *steeringDestination;
        position = &slot->position;
        steeringDestination = &slot->destination;
        slot->direction = Math_Atan2(slot->destination.x - slot->position.x,
                                     steeringDestination->z - position->z);
        table = gRandomTable;
        jitter = (SceneActorEffects_NextSample(table) & 3) - 1;
        slot->direction += jitter;
        slot->steeringTimer = (SceneActorEffects_NextSample(table) & 7) + 8;
        destination = steeringDestination;
    } else {
        slot->steeringTimer--;
        destination = &slot->destination;
        position = &slot->position;
    }
    {
        struct ContactPosition *write = &delta;
        u32 direction = slot->direction;
        u32 speed = slot->speed;
        write->x = truncateShift12(gUnk_086149C4[(direction + 64) & 255] * speed);
        write->y = 0;
        write->z = truncateShift12(gUnk_086149C4[direction] * speed);
    }
    {
        const struct ContactPosition *read = &delta;
        const struct ContactPosition *first = &delta;
        slot->position.x += first->x;
        position->y += read->y;
        position->z += read->z;
    }
    ContactActor_SetDirectionalSequence(pool, slot, 0);
    {
        s32 dx = slot->destination.x - slot->position.x;
        s32 dz = destination->z - position->z;
        if (((dx * dx) + (dz * dz)) <= 16383) {
            slot->state = 7;
            slot->enter = 1;
            slot->stateFrame = 0;
        } else
            slot->stateFrame++;
    }
}
