// MATCHING_CONCESSION: register-pinning
#include "contact_actor.h"
void ContactActor_SetDirectionalSequence(struct ContactActorPool *, struct ContactActor *, u32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
static inline s32 truncateShift12(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -((-value) >> 12);
}
static inline s32 cellIndex(struct ContactPosition *position)
{
    s32 x = position->x >> 8, z = position->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        return 0;
    return gTerrainMap->rows[z] + x;
}
static inline u8 *cell(s32 index)
{
    u8 *dynamic = TerrainModifiers_FindCellOverride(index, 1);
    if (dynamic)
        return dynamic + 4;
    return gTerrainMap->cells + 12 + index * 4;
}
static inline u32 nextRandom(const u16 *table)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return table[index];
}
static inline s32 terrainHeight(struct ContactPosition *position, s32 index)
{
    u8 *properties = cell(index);
    u32 type = *properties >> 4;
    s32 height = (*properties & 15) << 8;
    switch (type) {
        case 1:
            height -= (u8)position->z;
            break;
        case 2:
            height -= (u8)position->x;
            break;
    }
    return height;
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
        register u32 rngSetup asm("r8");
        const u16 *table;
        const u16 *earlyTable;
        u32 angle;
        register u32 angleMask asm("r6");
        register u32 radiusBits asm("r1");
        u32 radius;
        s32 jitter;
        struct ContactPosition *write;
        slot->enter = 0;
        {
            struct ContactCollider *collider = &slot->healthCollider;
            collider->flags |= 4;
        }
        center = &pool->retreatCenter;
        pool->retreatCenter.y = terrainHeight(center, cellIndex(center));
        rngSetup = 0x0403B714;
        asm volatile("" : : "r"(rngSetup));
        earlyTable = (const u16 *)0x0203B400;
        angle = nextRandom(earlyTable);
        angleMask = 255;
        angle &= angleMask;
        radiusBits = nextRandom(earlyTable) & 127;
        write = &delta;
        radius = radiusBits + 16;
        write->x = truncateShift12(gUnk_086149C4[(angle + 64) & angleMask] * radius);
        write->y = 0;
        write->z = truncateShift12(gUnk_086149C4[angle] * radius);
        entryDestination = &slot->destination;
        addPosition(entryDestination, &delta, &pool->retreatCenter);
        table = gRandomTable;
        slot->targetHeight = (nextRandom(table) & 511) + slot->desiredHeight;
        slot->direction = Math_Atan2(slot->destination.x - slot->position.x,
                                     entryDestination->z - slot->position.z);
        jitter = (nextRandom(table) & 15) - 8;
        slot->direction += jitter;
        slot->steeringTimer = (nextRandom(table) & 7) + 8;
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
        jitter = (nextRandom(table) & 3) - 1;
        slot->direction += jitter;
        slot->steeringTimer = (nextRandom(table) & 7) + 8;
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
