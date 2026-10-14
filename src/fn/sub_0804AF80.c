#include "tether_actor.h"
// MATCHING_CONCESSION: register-pinning
#define TetherActor_UpdateWorldPosition sub_0804AF80
void TetherActor_HandleFlaggedCell(struct TetherActor *);
void TetherActor_UpdateSampledPosition(struct TetherActor *);
u8 *sub_0821D64C(s32, s32);
void sub_0821BC68(void *, void *);
void sub_08225B74(void *);
static inline s32 cellIndex(s32 x, s32 z)
{
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}
static inline u8 *cellProperties(s32 cell)
{
    u8 *properties = sub_0821D64C(cell, 1);
    if (properties)
        properties += 4;
    else {
        struct EngineMap *map = gUnk_030052F4;
        u32 offset = cell * 4 + 12;
        properties = map->cells + offset;
    }
    return properties;
}
void TetherActor_UpdateWorldPosition(struct TetherActor *actor)
{
    struct ContactPosition *position;
    u32 oldCell;
    u32 oldSlope;
    u32 oldHeight;
    u32 oldAttributes;
    u32 offset;
    if (!actor->statusFlags && actor->state != 5) {
        s32 cell;
        u8 *properties;
        u32 surface, height;
        position = &actor->position;
        cell = cellIndex(position->x >> 8, position->z >> 8);
        properties = cellProperties(cell);
        surface = *properties >> 4;
        height = (*properties & 15) << 8;
        switch (surface) {
        case 1: height -= (u8)position->z; break;
        case 2: height -= (u8)position->x; break;
        }
        actor->position.y = height;
    }
    {
        struct DisplayPosition **slot = &actor->display.animation.position;
        u16 *display = (u16 *)*slot;
        register u32 value asm("r0");
        register u32 coordinate asm("r3");
        position = &actor->position;
        offset = 0x37E;
        value = *(u16 *)((u8 *)actor + offset);
        coordinate = (u16)position->x;
        display[0] = value + coordinate;
        asm("" ::: "memory");
        display = (u16 *)*slot;
        {
            u16 *axis = (u16 *)&actor->position.y;
            offset += 2;
            value = *(u16 *)((u8 *)actor + offset);
            coordinate = *axis;
            display[1] = value + coordinate;
        }
        asm("" ::: "memory");
        display = (u16 *)*slot;
        {
            u16 *axis = (u16 *)&actor->position.z;
            value = (u16)actor->shakeZ;
            {
                u32 z = *axis;
                display[2] = value + z;
            }
        }
        offset = 0x130;
        Collider_SetPosition((u8 *)actor + offset, position, 0);
    }
    oldCell = actor->movementProbeStorage.cell;
    oldSlope = actor->movementProbeStorage.slope;
    oldHeight = actor->movementProbeStorage.height;
    offset -= 2;
    oldAttributes = *(u16 *)((u8 *)actor + offset);
    if (actor->kind == 9) {
        if (!actor->movementProbe) {
            sub_0821BC68(&actor->movementProbeStorage, position);
            actor->movementProbe = &actor->movementProbeStorage;
        }
        sub_08225B74(&actor->bodyConfig);
    } else {
        u8 *properties;
        u32 currentCell;
        u32 index = cellIndex(position->x >> 8, position->z >> 8);
        {
            u16 *field = &actor->movementProbeStorage.cell;
            *field = index;
            currentCell = *field;
        }
        {
            u8 *raw = sub_0821D64C(currentCell, 1);
            if (raw)
                properties = raw + 4;
            else {
                struct EngineMap *map = gUnk_030052F4;
                u32 offset = currentCell * 4 + 12;
                properties = map->cells + offset;
            }
        }
        actor->movementProbeStorage.slope = *properties >> 4;
        {
            u32 byte = *properties;
            register u32 height asm("r0") = 15;
            register u32 heightOffset asm("r4");
            height &= byte;
            heightOffset = 0x129;
            *(u8 *)((u8 *)actor + heightOffset) = height;
            asm("" : : "r"(heightOffset));
        }
        actor->movementProbeStorage.attributes = *(u16 *)(properties + 2);
    }
    actor->movementProbeStorage.previousCell = oldCell;
    {
        register u32 slopeOffset asm("r3") = 0x12A;
        u8 *destination = (u8 *)actor + slopeOffset;
        register u32 sample asm("r4") = oldSlope;
        *destination = sample;
        {
            u32 heightOffset = 0x128;
            register u32 height asm("r2");
            destination = (u8 *)actor + heightOffset;
            height = oldHeight;
            *destination = height;
        }
        slopeOffset += 2;
        destination = (u8 *)actor + slopeOffset;
        sample = oldAttributes;
        *(u16 *)destination = sample;
    }
    if (actor->kind == 9)
        TetherActor_HandleFlaggedCell(actor);
    TetherActor_UpdateSampledPosition(actor);
    {
        register u32 offset asm("r0") = 0x3F0;
        struct DisplayPosition *target;
        const struct DisplayPosition *source = (const struct DisplayPosition *)&actor->position;
        register u32 first asm("r0");
        u32 second;
        asm("" : "+r"(offset));
        target = (struct DisplayPosition *)((u8 *)actor + offset);
        first = source->words[0];
        second = *(u32 *)((u8 *)actor + 0x54);
        target->words[0] = first;
        target->words[1] = second;
    }
    {
        register u32 headingOffset asm("r1") = 0x260;
        u32 heading;
        asm("" : "+r"(headingOffset));
        {
            register u8 *headingAddress asm("r0") = (u8 *)actor + headingOffset;
            heading = *headingAddress;
        }
        actor->targetPosition.pad = heading;
    }
}
