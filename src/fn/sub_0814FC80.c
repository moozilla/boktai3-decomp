#include "global.h"
#include "scene_vec.h"
// confidence=low: role inferred from callers and callback phases.
// confidence=low: approaches the scripted planar destination and returns to the standing update.
extern const s16 gUnk_086149C4[];
s32 Math_Atan2(s32, s32);
s32 Player_GetMovementRateScale(s32);
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(void *, s32, s32, s32);
void PlayerCue_EnterStandUpdate(void *);
void PlayerCue_CompleteAction(void *);
void Sound_PlaySong(u32);
struct PlayerScriptDestination {
    u8 unknown0[0x30];
    struct SceneVec position;
    s16 velocityX;
    u16 unknown3A;
    s16 velocityZ;
    u8 unknown3E[0x5A];
    u16 actionCueCounter;
    u8 unknown9A[6];
    u16 actionCueFlags;
    u8 unknownA2[0x1E];
    u8 movementSpeedScale;
    u8 unknownC1[0x1CE];
    u8 velocityScaleAxis;
    u8 unknown290[0x112];
    u8 heading, side, facing;
    u8 unknown3A5[0x73];
    u8 actionMode;
    u8 unknown419[0x2F];
    u32 actionFlags;
    u8 unknown44C[0x28];
    u16 tetherStatusFlags;
    u8 unknown476[0xBE];
    u16 sceneMode, unknown536, active;
    u8 unknown53A[0x56];
    struct SceneVec scriptDestination;
    u8 unknown598[5];
    s8 targetFacing;
};
static inline s32 scale12(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}
static inline s32 scale6(s32 n)
{
    return n >= 0 ? n >> 6 : -((-n) >> 6);
}
static inline u32 flags(u32 *p, u32 mask)
{
    return *p & mask;
}
static inline u8 slowed(struct PlayerScriptDestination *p)
{
    s32 active = p->active;
    s32 result = 0;
    if (active && p->sceneMode == 5)
        result = 1;
    return result;
}
#ifdef NONMATCHING
// fuzzy: 99.8%
void Player_AdvanceScriptDestination(struct PlayerScriptDestination *p)
{
    s32 dx = (p->scriptDestination.x) - (p->position.x);
    s32 dz = (p->scriptDestination.z) - (p->position.z);
    s32 angle = Math_Atan2(dx, dz);
    s32 speed = 12;
    s32 footstepSound;
    Player_GetMovementRateScale(12);
    if (p && slowed(p))
        speed = (speed * p->movementSpeedScale) >> 6;
    if (((dx * dx) + (dz * dz)) <= (speed * speed)) {
        p->position.x = p->scriptDestination.x;
        p->position.z = p->scriptDestination.z;
        if ((p->targetFacing) >= 0) {
            p->facing = p->targetFacing;
            p->targetFacing = -1;
        } else {
            s32 shifted = (((u16)angle) + 0x70) >> 5;
            u32 mask = 7;
            mask &= shifted;
            p->facing = mask;
        }
        if (p->facing > 4) {
            p->heading = 8 - p->facing;
            p->side = 1;
        } else {
            p->heading = p->facing;
            p->side = 0;
        }
        Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 0), 64, 1);
        PlayerCue_EnterStandUpdate(p);
        PlayerCue_CompleteAction(p);
    } else {
        p->velocityX = scale12(gUnk_086149C4[(angle + 64) & 255] * speed);
        p->velocityZ = scale12(gUnk_086149C4[angle & 255] * speed);
        if (p->velocityScaleAxis == 1) {
            if ((p->velocityZ) < 0)
                p->velocityZ = scale6((p->velocityZ) * 40);
            else
                p->velocityZ = scale6((p->velocityZ) * 52);
        } else if (p->velocityScaleAxis == 2) {
            if ((p->velocityX) < 0)
                p->velocityX = scale6((p->velocityX) * 40);
            else
                p->velocityX = scale6((p->velocityX) * 52);
        }
        p->facing = ((((angle + 16) & 255) >> 5) + 3) & 7;
        if (p->facing > 4) {
            p->heading = 8 - p->facing;
            p->side = 1;
        } else {
            p->heading = p->facing;
            p->side = 0;
        }
        if ((!((p->actionCueFlags) & 3)) && (!(p->actionCueCounter))) {
            if ((!((p && (p->active)) && (!(p->sceneMode)))) &&
                (!(p && ((u8)flags(&p->actionFlags, 4))))) {
                footstepSound = 0x28E;
                if (p && slowed(p)) {
                    if (!(p->actionCueFlags))
                        Sound_PlaySong(0xC8);
                    else
                        Sound_PlaySong(0xC7);
                } else if ((p->tetherStatusFlags) || (!p->velocityScaleAxis)) {
                    if (!(p->actionCueFlags))
                        Sound_PlaySong(0xCA);
                    else
                        Sound_PlaySong(0xC9);
                } else if (!(p->actionCueFlags))
                    Sound_PlaySong(footstepSound);
                else
                    Sound_PlaySong(0x28D);
            }
        }
        Player_SetAnimation(p, ActorVariant_GetTableValue(p->actionMode, 1), 64, 1);
    }
}
#else
INCLUDE_ASM("asm/nonmatching", Player_AdvanceScriptDestination);
#endif
