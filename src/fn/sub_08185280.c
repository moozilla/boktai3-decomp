#include "global.h"

// Tentative: checks the X/Z Manhattan distance to the movement target.

struct ActorController2Vector {
    s16 x, y, z, pad;
};

struct ActorController2 {
    u8 unk00[0x24];
    struct ActorController2Vector position;
    u8 unk2C[0x1c];
    struct ActorController2Vector referencePosition;
};

static inline void ActorController_SubtractPositions(struct ActorController2Vector *out,
                                                     struct ActorController2Vector *a,
                                                     struct ActorController2Vector *b)
{
    out->x = a->x - b->x;
    out->y = a->y - b->y;
    out->z = a->z - b->z;
}

u32 ActorController_IsNearTarget(struct ActorController2 *actor)
{
    struct ActorController2Vector delta;
    ActorController_SubtractPositions(&delta, &actor->position, &actor->referencePosition);
    if (delta.x < 0)
        delta.x = -delta.x;
    if (delta.z < 0)
        delta.z = -delta.z;
    return delta.x + delta.z < 0xc0;
}
