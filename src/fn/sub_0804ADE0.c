// MATCHING_CONCESSION: register-pinning
#include "tether_actor.h"
#include "game_state.h"
u32 TetherActor_RequestKindNineAction(u32, u32);
u8 *sub_0821D64C(s32, s32);
s32 Mod(s32, s32);
s32 Div(s32, s32);
static inline u8 flaggedCellHandlingEnabled(void)
{
    u32 mask = 0x2000;
    if (gUnk_02000710->flags868 & mask)
        return TRUE;
    return FALSE;
}
void TetherActor_HandleFlaggedCell(struct TetherActor *actor)
{
    struct ContactPosition *position = &actor->position;
    s32 x = position->x >> 8;
    s16 *clampX;
    s32 z = position->z >> 8;
    s16 **positionZReference;
    s32 cell;
    u8 *properties;
    if ((((x < 0) || (z < 0)) || (((u32)x) >= gUnk_030052F8)) || (((u32)z) >= gUnk_030052FC)) {
        cell = 0;
    } else {
        cell = gUnk_030052F4->rows[z] + x;
    }
    properties = sub_0821D64C(cell, 1);
    if (properties) {
        properties += 4;
    } else {
        struct EngineMap *map = gUnk_030052F4;
        u32 offset = (cell * 4) + 12;
        properties = map->cells + offset;
    }
    {
        u32 attributes = *((u16 *)(properties + 2));
        attributes &= 128;
        if (!attributes) {
            goto done;
        }
    }
    if (!flaggedCellHandlingEnabled()) {
        goto done;
    }
    {
        s32 column =
            Mod(actor->movementProbeStorage.previousCell, ((s16 *)gUnk_030052F4->cells)[2]);
        s32 row = Div(actor->movementProbeStorage.previousCell, ((s16 *)gUnk_030052F4->cells)[2]);
        s32 cellX = column << 8;
        s32 minX = cellX + 1;
        s32 maxX = cellX + 255;
        s32 cellZ = row << 8;
        s32 minZ = cellZ + 1;
        s32 maxZ = cellZ + 255;
        register s16 *positionX asm("r6") = &actor->position.x;
        s16 *positionZ;
        s32 pointX = *positionX;
        s32 dx;
        s32 dz;
        pointX -= 127;
        clampX = positionX;
        dx = cellX - pointX;
        positionZ = &actor->position.z;
        pointX = *positionZ;
        pointX -= 127;
        dz = cellZ - pointX;
        {
            register s32 direction asm("r0") = (u8)Math_Atan2(dx, dz);
            s32 angle;
            direction = (direction - 32) & (~63);
            asm("" : "+r"(direction));
            angle = direction;
            angle += 64;
            if ((*clampX) < minX) {
                *clampX = minX;
            } else if ((*clampX) > maxX) {
                *clampX = maxX;
            }
            positionZReference = &positionZ;
            if ((*(*positionZReference)) < minZ) {
                *positionZ = minZ;
            } else if ((*positionZ) > maxZ) {
                *(*positionZReference) = maxZ;
            }
            TetherActor_RequestKindNineAction(100, angle);
            if (actor->requestMode) {
                Sound_PlaySong(0x3F4);
                Sound_PlaySong(0x4ED);
                --actor->requestMode;
            }
            actor->requestStatus = 1;
            if (actor->onActionScriptId) {
                Script_Call(actor->onActionScriptId, 0);
            }
        }
    }
done:;
}
