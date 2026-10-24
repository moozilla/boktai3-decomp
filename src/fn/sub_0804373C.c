// MATCHING_CONCESSION: register-pinning
#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Reviewed full extent; register lifetimes and instruction scheduling still differ.
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
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}
static inline u8 *cell(s32 index)
{
    u8 *dynamic = TerrainModifiers_FindCellOverride(index, 1);
    if (dynamic)
        return dynamic + 4;
    return gUnk_030052F4->cells + 12 + index * 4;
}
static inline u32 nextRandom(const u16 *table)
{
    u32 index = (gUnk_03005308 + 1) & 1023;
    gUnk_03005308 = index;
    return table[index];
}
inline static u32 nextRandom_UpdateRetreat(const u16 *table)
{
    u32 index = ((*((typeof(gUnk_03005308) *)0x03005308)) + 1) & 1023;
    *((typeof(gUnk_03005308) *)0x03005308) = index;
    return table[index];
}

void ContactActor_UpdateRetreat(struct ContactActorPool *pool, struct ContactActor *slot, s32 index)
{
    struct ContactPosition delta;
    struct ContactPosition *position;
    struct ContactPosition *destination;
    if (slot->enter) {
        struct ContactPosition *center;
        u32 type;
        u32 packed;
        s32 height;
        register u32 observed asm("r8");
        const u16 *table;
        u32 angle;
        u32 radius;
        struct ContactPosition *write;
        slot->enter = 0;
        {
            struct ContactCollider *collider = &slot->healthCollider;
            collider->flags |= 4;
        }
        center = &pool->retreatCenter;
        packed = *cell(cellIndex(center));
        type = packed >> 4;
        height = (packed & 15) * 256;
        switch (type) {
            case 1:
                height -= (u8)center->z;
                break;

            case 2:
                height -= (u8)center->x;
                break;
        }

        pool->retreatCenter.y = height;
        observed = 0x0403B714;
        asm volatile("" : : "r"(observed));
        table = (const u16 *)0x0203B400;
        angle = nextRandom_UpdateRetreat(table) & 255;
        radius = (nextRandom_UpdateRetreat(table) & 127) + 16;
        write = &delta;
        write->x = truncateShift12(gUnk_086149C4[(angle + 64) & 255] * radius);
        write->y = 0;
        write->z = truncateShift12(gUnk_086149C4[angle] * radius);
        destination = &slot->destination;
        {
            const struct ContactPosition *read = &delta;
            const struct ContactPosition *first = &delta;
            slot->destination.x = first->x + pool->retreatCenter.x;
            destination->y = read->y + center->y;
            destination->z = read->z + center->z;
        }
        slot->targetHeight = (nextRandom_UpdateRetreat(table) & 511) + slot->desiredHeight;
        slot->direction =
            Math_Atan2(slot->destination.x - slot->position.x, destination->z - slot->position.z);
        slot->direction += (nextRandom_UpdateRetreat(table) & 15) - 8;
        slot->steeringTimer = (nextRandom_UpdateRetreat(table) & 7) + 8;
        slot->speed = 16;
    }
    if (!slot->steeringTimer) {
        const u16 *table;
        position = &slot->position;
        destination = &slot->destination;
        slot->direction =
            Math_Atan2(slot->destination.x - slot->position.x, destination->z - position->z);
        do {
            table = (const u16 *)0x0203B400;
        } while (0);
        slot->direction += (nextRandom_UpdateRetreat(table) & 3) - 1;
        slot->steeringTimer = (nextRandom_UpdateRetreat(table) & 7) + 8;
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

#else
#include "global.h"
INCLUDE_ASM("asm/nonmatching", ContactActor_UpdateRetreat);
#endif
