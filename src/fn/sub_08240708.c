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

void Player_UpdateRecoil(struct Player *p)
{
    if (p->tetherMode != 0x1F) {
        u8 *variant = (u8 *)p + 0x418;
        Player_SetAnimationIndex(p, ActorVariant_GetTableValue(*variant, 0x29), 0x40);
        p->interactionResetCounter = 0;
        if (*variant == 6)
            Player_PlayLocalSong(p->slot, 0x2EB);
        else
            Player_PlayLocalSong(p->slot, 0xE7);
        Player_PlayLocalSong(p->slot, 0xE6);
        Player_SetTetherMode(p, 0x1F, 0);
    }
    switch (p->tetherPhase) {
        case 0: {
            u32 a = ((p->facing + 1) & 7) << 5;
            struct StackPoint pos;
            s32 x, z;
            u32 cell;
            u8 *data;
            p->velocityX = Truncate12(gUnk_086149C4[(a + 0x40) & 0xFF] * p->directionFrame);
            p->velocityZ = Truncate12(gUnk_086149C4[a] * p->directionFrame);
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
            data = TerrainModifiers_FindCellOverride(cell, 1);
            if (data)
                data += 4;
            else
                data = gTerrainMap->cells + (cell * 4 + 12);
            if (Player_IsFlaggedWithGlobalModifier10(*(u16 *)(data + 2))) {
                p->velocityX = 0;
                p->velocityZ = 0;
            }
            if (p->directionFrame == 0)
                Player_SetTetherMode(p, 0x1F, 1);
            break;
        }
        case 1:
            if (p->actionCueFlags == 3 && p->actionCueCounter == 0) {
                Player_PlayLocalSong(p->slot, 0xD1);
                SelectionSlots_ClearValue(&p->position, p->slot);
            }
            if (Player_SetAnimationIndex(p, ActorVariant_GetTableValue(*((u8 *)p + 0x418), 0x2E),
                                         0x40))
                Player_SetTetherMode(p, 0x1F, 2);
            break;
        case 2:
            if (++p->tetherFrame > 0xF)
                Player_SetTetherMode(p, 0x1F, 3);
            break;
        case 3:
            if ((p->tetherFrame & 7) >= 7 - (p->tetherFrame >> 3))
                Player_SetSecondaryActionFlags(p, 5);
            if (++p->tetherFrame > 0x37)
                sub_0823C280(p);
            break;
    }
}
