#include "global.h"
#include "player.h"
#include "engine.h"
#define Player_UpdateAirborneKnockback sub_08240408
// confidence=low: decaying reverse velocity and height suggest airborne knockback.
struct RecoveryVec {
    s32 x : 16, y : 16, z : 16, pad : 16;
};
u32 ActorVariant_GetTableValue(u32, u32);
u32 sub_08238280(void *, u32, u32);
void Player_PlayLocalSong(u32, u32);
void Player_SetTetherMode(void *, u32, u32);
void Player_SetSecondaryActionFlags(void *, u32);
void sub_0823C280(void *);
void sub_08109040(void *, u32);
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

void sub_08238238(void *);
// Remaining differences: speed-address lifetime, scratch registers, and saved-register frame.
#ifdef NONMATCHING
// fuzzy: 85.8%
void Player_UpdateAirborneKnockback(struct Player *p)
{
    if (p->tetherMode != 0x1E) {
        Player_SetTetherMode(p, 0x1E, 0);
        if (p->facing > 4) {
            p->heading = (8 - p->facing) >> 1;
            p->side = 1;
        } else {
            p->heading = p->facing >> 1;
            p->side = 0;
        }
        if (p->contactActionTag) {
            Player_PlayLocalSong(p->slot, p->contactActionTag);
            p->contactActionTag = 0;
        }
    }
    switch (p->tetherPhase) {
        case 0:
            sub_08238280(p, ActorVariant_GetTableValue(p->actionMode, 0x29), 0x40);
            if (++p->tetherFrame > 2)
                Player_SetTetherMode(p, 0x1E, 1);
            break;
        case 1:
            if (!p->tetherFrame)
                p->terrainRecoveryCounter = 300;
            sub_08238280(p, ActorVariant_GetTableValue(p->actionMode, 0x2B), 0x40);
            reverseMotion(p, &p->directionFrame);
            p->directionFrame = (p->directionFrame * 3) >> 2;
            if (p->tetherFrame > 3) {
                if (p->terrainRecoveryCounter >= (p->tetherFrame << 3))
                    p->terrainRecoveryCounter -= p->tetherFrame << 3;
                else
                    p->terrainRecoveryCounter = 0;
            }
            ++p->tetherFrame;
            if (p->directionFrame <= 19 && p->terrainRecoveryCounter == 0) {
                p->terrainRecoveryCounter = 0;
                Player_PlayLocalSong(p->slot, 0x2A8);
                Player_SetTetherMode(p, 0x1E, 2);
            }
            break;
        case 2:
            reverseMotion(p, &p->directionFrame);
            if (sub_08238280(p, ActorVariant_GetTableValue(p->actionMode, 0x2C), 0x40)) {
                p->directionFrame = 0;
                Player_SetTetherMode(p, 0x1E, 3);
            }
            break;
        case 3:
            if (++p->tetherFrame > 19)
                Player_SetTetherMode(p, 0x1E, 4);
            break;
        case 4:
            if (sub_08238280(p, ActorVariant_GetTableValue(p->actionMode, 0x2D), 0x40)) {
                *(u8 *)((u8 *)p + 0x4BA) = 0;
                sub_08238238(p);
                Player_SetTetherMode(p, 0, 0);
            }
            break;
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_08240408);
#endif
