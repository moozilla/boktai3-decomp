#include "global.h"
#include "player.h"
#include "engine.h"
// confidence=low: reverse velocity, shake, and target-directed recoil after impact.
struct RecoveryVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
u32 ActorVariant_GetTableValue(u32, u32);
u32 Player_SetAnimationIndex(void *, u32, u32);
void Player_PlayLocalSong(u32, u32);
void Player_SetTetherMode(void *, u32, u32);
void Player_SetSecondaryActionFlags(void *, u32);
void sub_0823C280(void *);
void SelectionSlots_ClearValue(void *, u32);
u8 *TerrainModifiers_FindCellOverride(s32, s32);
u32 Player_IsFlaggedWithGlobalModifier10(u32);
static inline s32 scale(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}
static inline void reverseMotion(struct Player *p, u16 *magnitude)
{
    s32 angle = ((p->facing + 5) & 7) << 5;
    p->velocityX = -scale(gUnk_086149C4[(angle + 64) & 255] * *magnitude);
    p->velocityZ = -scale(gUnk_086149C4[angle] * p->directionFrame);
}

void RisingSprite_Spawn(void *);
void Camera_RequestShake(u32);
void sub_08238238(void *);
static inline s32 scale3(s32 v)
{
    if (v >= 0)
        return v >> 3;
    return -((-v) >> 3);
}
void Player_UpdateTetherRecoil(struct Player *p)
{
    if (p->tetherMode != 0x1D) {
        Player_SetTetherMode(p, 0x1D, 0);
        if (p->facing > 4) {
            p->heading = (8 - p->facing) >> 1;
            p->side = 1;
        } else {
            p->heading = p->facing >> 1;
            p->side = 0;
        }
        Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x29), 0x40);
        if (p->contactActionTag) {
            Player_PlayLocalSong(p->slot, p->contactActionTag);
            p->contactActionTag = 0;
        }
    }
    {
        u16 *magnitude = &p->directionFrame;
        if (*magnitude) {
            if (*(u8 *)((u8 *)p + 0x479)) {
                s32 dx = p->terrainTarget->x - p->position.x;
                s32 dz = p->terrainTarget->z - p->position.z;
                p->velocityX = scale3(dx);
                p->velocityZ = scale3(dz);
                if (p->tetherFrame <= 5) {
                    if (!(p->tetherFrame & 1)) {
                        Player_PlayLocalSong(p->slot, 0x191);
                        RisingSprite_Spawn(&p->position);
                    }
                    ++p->tetherFrame;
                }
            } else
                reverseMotion(p, magnitude);
            Camera_RequestShake(p->directionFrame);
            p->directionFrame = (p->directionFrame * 3) >> 2;
        } else {
            *magnitude = 0;
            *(u8 *)((u8 *)p + 0x479) = 0;
            if (p->movementModifierKind == 3 && p->movementModifierActive)
                Player_SetTetherMode(p, 0x16, 0);
            else if (Player_SetAnimationIndex(p, ActorVariant_GetTableValue(p->actionMode, 0x2A),
                                              0x40)) {
                sub_08238238(p);
                Player_SetTetherMode(p, 0, 0);
            }
        }
    }
}
