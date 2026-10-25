#include "global.h"
#include "engine.h"
#include "contact_actor.h"

struct MovingParticle {
    u32 flags;
    u8 unk_04[0x14];
    struct ContactPackedVector position;
    u8 unk_20[8];
    struct ContactPosition velocity;
    u16 motionFlags;
    u8 state;
    u8 index;
    u8 reset;
    u8 unk_35;
    u8 animationTimer;
    u8 animationFrame;
    u16 lifetime;
    u16 cell;
    u8 frame;
};

u8 *TerrainModifiers_FindCellOverride(s32, s32);
s32 Div(s32, s32);
void SceneMovingParticle_UpdateAnimation(void *, struct MovingParticle *);

static inline s32 CellIndex(struct ContactPackedVector *position)
{
    s32 x = position->x >> 8;
    s32 z = position->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        return 0;
    return gUnk_030052F4->rows[z] + x;
}

static inline s32 GroundHeight(s32 cell, struct ContactPackedVector *position)
{
    u8 *definition = TerrainModifiers_FindCellOverride(cell, 1);
    u32 kind;
    s32 height;
    if (definition)
        definition += 4;
    else {
        struct EngineMap *map = gUnk_030052F4;
        u32 offset = cell * 4 + 12;
        definition = map->cells + offset;
    }
    kind = *definition >> 4;
    height = (*definition & 15) << 8;
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

static inline u8 Move(struct ContactPackedVector *position, struct ContactPosition *velocity,
                      s32 cell)
{
    s32 height;
    struct ContactPackedVector target;
    s32 targetCell;
    Math_Atan2(velocity->x, velocity->z);
    height = GroundHeight(cell, position);
    if (velocity->x * velocity->x + velocity->y * velocity->y + velocity->z * velocity->z == 0)
        return 1;
    if (position->y <= height) {
        velocity->x = Div(velocity->x * 6, 10);
        velocity->y = Div(-(velocity->y * 4), 10);
        velocity->z = Div(velocity->z * 6, 10);
        position->y = height;
    }
    target.x = position->x + velocity->x;
    target.y = position->y + velocity->y;
    target.z = position->z + velocity->z;
    targetCell = CellIndex(&target);
    if (cell != targetCell) {
        if (height < GroundHeight(targetCell, &target)) {
            velocity->x = -Div(velocity->x * 5, 10);
            velocity->z = -Div(velocity->z * 5, 10);
        }
    }
    position->x += velocity->x;
    position->y += velocity->y;
    position->z += velocity->z;
    return 0;
}

void SceneMovingParticle_Update(void *manager, struct MovingParticle *particle)
{
    if (particle->reset) {
        particle->flags &= ~1;
        particle->reset = 0;
    }
    if (!(particle->motionFlags & 1)) {
        particle->cell = CellIndex(&particle->position);
        particle->velocity.y--;
        if (Move(&particle->position, &particle->velocity, particle->cell))
            particle->motionFlags |= 1;
    }
    SceneMovingParticle_UpdateAnimation(manager, particle);
    if (particle->lifetime)
        particle->lifetime--;
    else {
        particle->state = 2;
        particle->reset = 1;
    }
}
