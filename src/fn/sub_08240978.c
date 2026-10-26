#include "global.h"
#include "engine.h"
#include "game_state.h"
#include "player.h"

extern const s16 gUnk_086149C4[];

struct StackPoint {
    u32 x : 16, y : 16, z : 16, pad : 16;
};

static inline s32 Truncate12(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}

u8 *TerrainModifiers_FindCellOverride(s32, s32);
u32 Player_IsFlaggedWithGlobalModifier10(u32);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimationIndex(struct Player *, s32, s32);
void Player_SetTetherMode(struct Player *, s32, s32);
void Player_PlayLocalSong(s32, s32);
void Player_SetSecondaryActionFlags(struct Player *, u32);
void SelectionSlots_ClearValue(void *, s32);
void sub_0823C280(struct Player *);
void sub_08238238(struct Player *);

static inline u8 *CellData(u32 cell)
{
    u8 *data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        return data + 4;
    return gTerrainMap->cells + (cell * 4 + 12);
}

void Player_UpdateFlinch(struct Player *p)
{
    if (p->tetherMode != 0x20) {
        if (p->facing > 4) {
            p->heading = (8 - p->facing) >> 1;
            p->side = 1;
        } else {
            p->heading = p->facing >> 1;
            p->side = 0;
        }
        if (*((u8 *)p + 0x418) == 6)
            Player_PlayLocalSong(p->slot, 0x2EB);
        else
            Player_PlayLocalSong(p->slot, 0xE7);
        Player_PlayLocalSong(p->slot, 0xE6);
        Player_SetTetherMode(p, 0x20, 0);
    }
    switch (p->tetherPhase) {
        case 0:
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(*((u8 *)p + 0x418), 0x29), 0x40);
            if (++p->tetherFrame > 3) {
                Player_SetTetherMode(p, 0x20, 1);
                p->terrainRecoveryCounter = 300;
            }
            break;
        case 1: {
            u32 a;
            struct StackPoint pos;
            s32 x, z;
            u32 cell;
            Player_SetAnimationIndex(p, ActorVariant_GetTableValue(*((u8 *)p + 0x418), 0x2B), 0x40);
            a = ((p->facing + 5) & 7) << 5;
            p->velocityX = -Truncate12(gUnk_086149C4[(a + 0x40) & 0xFF] * p->directionFrame);
            p->velocityZ = -Truncate12(gUnk_086149C4[a] * p->directionFrame);
            p->directionFrame = p->directionFrame * 3 >> 2;
            pos = *(struct StackPoint *)&p->position;
            pos.x += p->velocityX;
            pos.z += p->velocityZ;
            x = ((struct SceneVec *)&pos)->x >> 8;
            z = ((struct SceneVec *)&pos)->z >> 8;
            if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
                cell = 0;
            else
                cell = gTerrainMap->rows[z] + x;
            if (Player_IsFlaggedWithGlobalModifier10(*(u16 *)(CellData(cell) + 2))) {
                p->velocityX = 0;
                p->velocityZ = 0;
            }
            if (p->terrainRecoveryCounter > 14)
                p->terrainRecoveryCounter -= 15;
            else
                p->terrainRecoveryCounter = 0;
            if (p->directionFrame <= 0x13) {
                p->terrainRecoveryCounter = 0;
                Player_PlayLocalSong(p->slot, 0x2A8);
                SelectionSlots_ClearValue(&p->position, p->slot);
                Player_SetTetherMode(p, 0x20, 2);
            }
            break;
        }
        case 2: {
            u32 a = ((p->facing + 5) & 7) << 5;
            struct StackPoint pos;
            s32 x, z;
            u32 cell;
            p->velocityX = -Truncate12(gUnk_086149C4[(a + 0x40) & 0xFF] * p->directionFrame);
            p->velocityZ = -Truncate12(gUnk_086149C4[a] * p->directionFrame);
            pos = *(struct StackPoint *)&p->position;
            pos.x += p->velocityX;
            pos.z += p->velocityZ;
            x = ((struct SceneVec *)&pos)->x >> 8;
            z = ((struct SceneVec *)&pos)->z >> 8;
            if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
                cell = 0;
            else
                cell = gTerrainMap->rows[z] + x;
            if (Player_IsFlaggedWithGlobalModifier10(*(u16 *)(CellData(cell) + 2))) {
                p->velocityX = 0;
                p->velocityZ = 0;
            }
            if (Player_SetAnimationIndex(p, ActorVariant_GetTableValue(*((u8 *)p + 0x418), 0x2C),
                                         0x40)) {
                p->directionFrame = 0;
                Player_SetTetherMode(p, 0x20, 3);
            }
            break;
        }
        case 3:
            if (++p->tetherFrame > 0xF)
                Player_SetTetherMode(p, 0x20, 4);
            break;
        case 4:
            if ((p->tetherFrame & 7) >= 7 - (p->tetherFrame >> 3))
                Player_SetSecondaryActionFlags(p, 5);
            if (++p->tetherFrame > 0x37)
                sub_0823C280(p);
            break;
    }
}
