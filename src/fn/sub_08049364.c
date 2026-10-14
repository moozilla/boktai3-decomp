// MATCHING_CONCESSION: register-pinning
#include "tether_actor.h"
void TetherActor_CreateAuxiliarySprite(struct TetherActor *);
void TetherActor_SetState(struct TetherActor *, u32);
void TetherActor_UpdatePendingEffect(struct TetherActor *);
u8 *sub_0821D64C(s32, s32);
void sub_08072C40(void *);
void TetherActor_BeginAuxiliaryState(struct TetherActor *input)
{
    struct TetherActor *actor = input;
    struct ContactPosition *position;
    s32 x, z;
    s32 cell;
    u8 *properties;
    u32 surface, height;
    actor->auxiliaryOrigin = actor->position;
    actor->auxiliaryOrigin.y += 384;
    TetherActor_CreateAuxiliarySprite(actor);
    actor->auxiliarySprite.position = actor->position;
    position = &actor->auxiliarySprite.position;
    x = position->x >> 8;
    z = position->z >> 8;
    if (x < 0 || z < 0 || (u32)x >= gUnk_030052F8 || (u32)z >= gUnk_030052FC)
        cell = 0;
    else {
        u32 row = gUnk_030052F4->rows[z];
        cell = row + x;
    }
    properties = sub_0821D64C(cell, 1);
    if (properties)
        properties += 4;
    else {
        struct EngineMap *map = gUnk_030052F4;
        register u32 offset asm("r0") = cell * 4 + 12;
        properties = map->cells + offset;
    }
    surface = *properties >> 4;
    height = (*properties & 15) << 8;
    switch (surface) {
        case 1:
            height -= (u8)position->z;
            break;
        case 2:
            height -= (u8)position->x;
            break;
    }
    actor->auxiliarySprite.position.y = height;
    if (actor->auxiliaryRegistered) {
        sub_08072C40(actor->auxiliaryRegistration);
        actor->auxiliaryRegistered = 0;
    }
    TetherActor_UpdatePendingEffect(actor);
    TetherActor_SetState(actor, 5);
}
