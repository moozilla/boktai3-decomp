// MATCHING_CONCESSION: register-pinning
#include "contact_sequence_actor.h"
struct ContactSequenceActor *ContactSequencePool_FindInactiveActor(struct ContactSequenceOwner *);
void ContactSequenceActor_SetState(struct ContactSequenceActor *, u32);
void *TerrainModifiers_FindCellOverride(u32, u32);
void Collider_SetAttackPowerAndForce(void *, u32, u32);
void Collider_SetBodyProperties(void *, u32, u32, u32);
struct ContactSequenceFrame {
    u16 frame, unknown2;
};
extern const struct ContactSequenceFrame gUnk_08605974[];
s32 ContactSequencePool_SpawnActor(struct ContactSequenceOwner *pool,
                                   const struct ContactPosition *position, u32 variant)
{
    u32 savedVariant;
    register struct ContactSequenceOwner *owner asm("r6");
    struct ContactSequenceActor *actor;
    struct ContactPosition *actorPosition;
    register struct ContactPosition *savedPosition asm("r10");
    s32 x, z;
    u32 cell;
    const u8 *entry;
    u32 height, slope;
    struct ContactCollider *healthCollider, *resourceCollider;
    savedVariant = variant;
    owner = pool;
    asm("" : "+r"(owner));
    if (!owner || !(actor = ContactSequencePool_FindInactiveActor(owner)))
        return -1;
    actor->position = *position;
    actorPosition = &actor->position;
    x = actor->position.x >> 8;
    z = actorPosition->z >> 8;
    savedPosition = actorPosition;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        cell = 0;
    else
        cell = gTerrainMap->rows[z] + x;
    entry = TerrainModifiers_FindCellOverride(cell, 1);
    if (entry)
        entry += 4;
    else {
        struct EngineMap *map = gTerrainMap;
        u32 offset = cell * 4 + 12;
        entry = map->cells + offset;
    }
    slope = *entry >> 4;
    height = (*entry & 15) << 8;
    switch (slope) {
        case 1:
            height -= (u8)actorPosition->z;
            break;
        case 2:
            height -= (u8)actorPosition->x;
            break;
    }
    actor->position.y = height;
    actor->sprite.position = actor->position;
    healthCollider = &actor->healthCollider;
    Collider_SetPosition(healthCollider, savedPosition, 0);
    resourceCollider = &actor->resourceCollider;
    Collider_SetPosition(resourceCollider, savedPosition, 0);
    Collider_SetAttackPowerAndForce(resourceCollider, (u16)owner->resourceValue, 50);
    Collider_SetBodyProperties(healthCollider, owner->contactValue, 0, 0);
    ContactSequenceActor_SetState(actor, 1);
    actor->stateFrame = 0;
    {
        register u32 zero asm("r2") = 0;
        actor->flash = zero;
    }
    actor->health = owner->initialHealth;
    {
        register u32 lowVariant asm("r5") = savedVariant;
        actor->variant = lowVariant;
    }
    {
        struct DisplaySequenceState *state = &actor->sequence;
        void *resource = (void *)owner->sequenceResource;
        const struct ContactSequenceFrame *frames = gUnk_08605974;
        u32 one = 1;
        u32 index;
        register u32 zero asm("r5");
        index = savedVariant;
        asm("" : "+r"(index));
        index &= one;
        Anim_SetSequence(state, resource, frames[index].frame, 0, 8);
        zero = 0;
        actor->cooldown = zero;
        actor->active = one;
    }
    return 0;
}
