#include "global.h"
#include "engine/engine.h"
#include "system/game_state/game_state.h"
#include "player/player.h"
#include "scene/terrain/terrain_cell_probe.h"
#include "scene/terrain/terrain_sampling.h"

extern const s16 gUnk_086149C4[];

static inline s32 Truncate12(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}

u32 Player_IsFlaggedWithGlobalModifier10(u32);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimationIndex(struct Player *, s32, s32);
void Player_SetTetherMode(struct Player *, s32, s32);
void Player_PlayLocalSong(s32, s32);
void Player_SetSecondaryActionFlags(struct Player *, u32);
void SelectionSlots_ClearValue(void *, s32);
void Player_RestoreLinkedActionState(struct Player *);

void Player_UpdateRecoil(struct Player *p)
{
    if (p->tetherMode != 0x1F) {
        Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x29), 0x40);
        p->interactionResetCounter = 0;
        if (p->actionMode == 6)
            Player_PlayLocalSong(p->slot, 0x2EB);
        else
            Player_PlayLocalSong(p->slot, 0xE7);
        Player_PlayLocalSong(p->slot, 0xE6);
        Player_SetTetherMode(p, 0x1F, 0);
    }
    switch (p->tetherPhase) {
        case 0: {
            u32 angle = ((p->facing + 1) & 7) << 5;
            struct ContactPosition pos;
            u32 cell;
            p->velocityX = Truncate12(gUnk_086149C4[(angle + 0x40) & 0xFF] * p->directionFrame);
            p->velocityZ = Truncate12(gUnk_086149C4[angle] * p->directionFrame);
            p->directionFrame = p->directionFrame * 3 >> 2;
            pos = *(struct ContactPosition *)&p->position;
            pos.x += p->velocityX;
            pos.z += p->velocityZ;
            cell = Terrain_GetCellIndex(&pos);
            if (Player_IsFlaggedWithGlobalModifier10(
                    ((struct TerrainCell *)cellProps(cell))->flags)) {
                p->velocityX = 0;
                p->velocityZ = 0;
            }
            if (p->directionFrame == 0)
                Player_SetTetherMode(p, 0x1F, 1);
            break;
        }
        case 1:
            if (p->animation.frameIndex == 3 && p->animation.elapsed == 0) {
                Player_PlayLocalSong(p->slot, 0xD1);
                SelectionSlots_ClearValue(&p->position, p->slot);
            }
            if (Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x2E), 0x40))
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
                Player_RestoreLinkedActionState(p);
            break;
    }
}
