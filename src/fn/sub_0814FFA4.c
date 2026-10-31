#include "global.h"
// confidence=low: MoveTo callback approaches destination and decelerates its movement effect.
extern const s16 gUnk_086149C4[];
s32 ActorVariant_GetTableValue(u32, s32);
s32 Player_SetAnimation(void *, s32, s32, s32);
s32 Math_Atan2(s32, s32);
void Script_Call(u32, u32);
void Player_ConstrainTetherTarget(void *, s32, s32, u32);
#include "scene_vec.h"
struct PlayerMoveDestination {
    u8 unknown0[0x30];
    struct SceneVec position;
    s16 velocityX;
    u16 unknown3A;
    s16 velocityZ;
    u8 unknown3E[0xA0 - 0x3E];
    u16 actionCueFlags;
    u8 unknownA2[0x28F - 0xA2];
    u8 velocityScaleAxis;
    u8 unknown290[0x3A2 - 0x290];
    u8 heading, side, facing;
    u8 unknown3A5[0x418 - 0x3A5];
    u8 actionMode;
    u8 unknown419[0x590 - 0x419];
    struct SceneVec scriptDestination;
    u8 unknown598[2];
    u8 animationBranchRequested, cuePhase, kindTwoEntered;
    s8 targetFacing;
    u8 unknown59E[6];
    u32 cueParameter;
};
static inline s32 scale12(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}
static inline s32 scale6(s32 n)
{
    return n >= 0 ? n >> 6 : -((-n) >> 6);
}
static inline void faceFromAngle(struct PlayerMoveDestination *p, u16 angle)
{
    s32 shifted = (((u16)angle) + 0x70) >> 5;
    u32 mask = 7;
    u8 direction;
    u8 *facing;
    mask &= shifted;
    facing = &p->facing;
    direction = mask;
    *facing = mask;
    if (direction > 4) {
        p->heading = 8 - direction;
        p->side = 1;
    } else {
        p->heading = mask;
        p->side = 0;
    }
}
void PlayerCue_UpdateMoveDestination(struct PlayerMoveDestination *p)
{
    s32 dx = (p->scriptDestination.x) - (p->position.x);
    s32 dz = (p->scriptDestination.z) - (p->position.z);
    s32 distance = (dx * dx) + (dz * dz);
    s32 x = 0;
    s32 z = 0;
    s32 moving;
    s32 anim;
    s32 angle;
    if (!distance) {
        anim = ActorVariant_GetTableValue(p->actionMode, 16);
        if ((p->targetFacing) >= 0) {
            p->facing = p->targetFacing;
            p->targetFacing = -1;
            {
                u32 direction = p->facing;
                if (direction > 4) {
                    p->heading = 8 - direction;
                    p->side = 1;
                } else {
                    p->heading = direction;
                    p->side = 0;
                }
            }
        }
        if (p->animationBranchRequested) {
            if (p->cuePhase) {
                moving = 1;
                angle = (((p->facing) + 1) & 7) << 5;
                x = gUnk_086149C4[(angle + 64) & 255];
                z = gUnk_086149C4[angle & 255];
                p->cuePhase = ((p->cuePhase) * 7) >> 3;
            } else {
                moving = 0;
                if (p->cueParameter) {
                    u32 script = p->cueParameter;
                    p->cueParameter = 0;
                    Script_Call(script, 0);
                }
                p->animationBranchRequested = 0;
            }
        } else {
            moving = 0;
            if (p->cueParameter) {
                u32 script = p->cueParameter;
                p->cueParameter = 0;
                Script_Call(script, 0);
            }
        }
    } else {
        anim = ActorVariant_GetTableValue(p->actionMode, 17);
        moving = 1;
        faceFromAngle(p, Math_Atan2(dx, dz));
        angle = (((p->facing) + 1) & 7) << 5;
        x = gUnk_086149C4[(angle + 64) & 255];
        z = gUnk_086149C4[angle & 255];
        if ((p->actionCueFlags) & 3) {
            if (distance <= 64) {
                p->position.x = p->scriptDestination.x;
                p->position.z = p->scriptDestination.z;
            } else {
                s32 speed = 8;
                p->velocityX = -scale12(x * speed);
                p->velocityZ = -scale12(z * speed);
                if ((p->velocityScaleAxis) == 1) {
                    if ((p->velocityZ) < 0)
                        p->velocityZ = scale6((p->velocityZ) * 40);
                    else
                        p->velocityZ = scale6((p->velocityZ) * 52);
                } else if ((p->velocityScaleAxis) == 2) {
                    if ((p->velocityX) < 0)
                        p->velocityX = scale6((p->velocityX) * 40);
                    else
                        p->velocityX = scale6((p->velocityX) * 52);
                }
            }
            if ((p->animationBranchRequested) && ((p->cuePhase) <= 149))
                p->cuePhase = ((p->cuePhase) * 15) >> 4;
        }
    }
    Player_SetAnimation(p, anim, 64, 1);
    if (moving)
        Player_ConstrainTetherTarget(p, x, z, p->cuePhase);
}
