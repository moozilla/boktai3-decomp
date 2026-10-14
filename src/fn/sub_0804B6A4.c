#include "tether_actor.h"
// MATCHING_CONCESSION: register-pinning
u8 *sub_0821D64C(s32, s32);
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
static inline s32 surfaceHeight(const struct ContactPosition *position)
{
    u8 *properties = cellProperties(cellIndex(position->x >> 8, position->z >> 8));
    u32 surface = *properties >> 4;
    s32 height = (*properties & 15) << 8;
    switch (surface) {
        case 1:
            height -= (u8)position->z;
            break;
        case 2:
            height -= (u8)position->x;
            break;
    }
    return height;
}
void TetherActor_BlockPlayerAcrossTerrain(struct TetherActor *actor)
{
    u8 *properties = cellProperties(
        cellIndex(gUnk_02000580[0]->position.x >> 8, gUnk_02000580[0]->position.z >> 8));
    u32 attributes = *(u16 *)(properties + 2);
    struct ContactPosition *position;
    register s32 playerCell asm("r4");
    s32 actorCell;
    register s32 rowStride asm("r1");
    attributes &= 1;
    position = &actor->position;
    if (attributes == 0) {
        s32 playerHeight = surfaceHeight(&gUnk_02000580[0]->position);
        s32 height, difference;
        asm("" : "+r"(actor));
        {
            struct ContactPosition *heightPosition = &actor->position;
            s32 x = heightPosition->x >> 8, z = heightPosition->z >> 8;
            u8 *properties;
            u32 surface;
            position = heightPosition;
            properties = cellProperties(cellIndex(x, z));
            surface = *properties >> 4;
            height = (*properties & 15) << 8;
            switch (surface) {
                case 1:
                    height -= (u8)heightPosition->z;
                    break;
                case 2:
                    height -= (u8)heightPosition->x;
                    break;
            }
        }
        difference = playerHeight - height;
        if (difference < 0)
            difference = -difference;
        if (difference <= 255)
            return;
    }
    {
        s32 x;
        s32 z;
        u32 raw;
        raw = (u16)gUnk_02000580[0]->position.x;
        x = (s16)raw >> 8;
        raw = (u16)gUnk_02000580[0]->position.z;
        z = (s16)raw >> 8;
        playerCell = cellIndex(x, z);
        raw = (u16)position->x;
        x = (s16)raw >> 8;
        raw = (u16)position->z;
        z = (s16)raw >> 8;
        actorCell = cellIndex(x, z);
    }
    if (actorCell == playerCell + 1) {
        gUnk_02000580[0]->position.x = ((position->x >> 8) << 8) + 2;
        return;
    }
    if (actorCell == playerCell - 1) {
        gUnk_02000580[0]->position.x = (((position->x >> 8) + 1) << 8) - 2;
        return;
    }
    rowStride = *(s16 *)(gUnk_030052F4->cells + 4);
    if (actorCell == playerCell + rowStride)
        gUnk_02000580[0]->position.z = ((actor->position.z >> 8) << 8) + 2;
    else if (actorCell == playerCell - rowStride)
        gUnk_02000580[0]->position.z = (((actor->position.z >> 8) + 1) << 8) - 2;
    else {
        asm("" : : "r"(rowStride), "r"(playerCell));
        gUnk_02000580[0]->position.x = position->x;
        gUnk_02000580[0]->position.z = actor->position.z;
    }
}
