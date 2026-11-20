#include "global.h"
#include "vulcan_cannon_worker.h"
#include "scene_vec.h"

struct SceneVec *Player_GetEffectivePosition(s32);
u32 Anim_RestartSequence(struct DisplaySequenceState *, void *, u16, u8, u8);
s32 VulcanCannon_HasClearShot(struct VulcanCannon *);
s32 Math_Atan2(s32, s32);
void VulcanCannon_UpdateFacingTurn(void);
void VulcanCannon_UpdateFiringBurst(void);
static inline s32 TargetDistance(struct SceneVec *player, struct ContactPosition *position)
{
    struct SceneVec *a = player;
    struct ContactPosition *b = position;
    s32 dx, dz;
    dx = a->x - b->x;
    dz = a->z - b->z;
    return dx * dx + dz * dz;
}
s32 VulcanCannon_UpdateTargeting(struct VulcanCannon *p)
{
    s32 facingRight = 0;
    struct SceneVec *player = Player_GetEffectivePosition(0);
    s32 distance = TargetDistance(player, &p->display.position);
    struct SceneVec target;

    SceneVec_Copy(&target, Player_GetEffectivePosition(0));
    if (p->delayScript) {
        if (p->timer > p->delays[p->delayIndex]) {
            if (p->facing == facingRight)
                Anim_RestartSequence(&p->sequence, p->resource, 0x1c, 0, 8);
            else
                Anim_RestartSequence(&p->sequence, p->resource, 0x1c, 0, 12);
            p->state = VulcanCannon_UpdateFacingTurn;
            p->timer = 0;
            p->delayIndex++;
            if (p->delayIndex > p->delayCount - 1)
                p->delayIndex = 0;
        }
    } else {
        if (p->timer > p->turnDelay && p->turnDelay != 0) {
            if (p->facing == facingRight)
                Anim_RestartSequence(&p->sequence, p->resource, 0x1c, 0, 8);
            else
                Anim_RestartSequence(&p->sequence, p->resource, 0x1c, 0, 12);
            p->timer = 0;
            p->state = VulcanCannon_UpdateFacingTurn;
        }
    }
    if (distance < p->range * p->range) {
        if (!p->facing) {
            if (target.z > p->display.position.z - 0x80 &&
                target.z < p->display.position.z + 0x80 && target.x > p->display.position.x) {
                if (VulcanCannon_HasClearShot(p)) {
                    p->aimTimer = 0;
                    p->targetVisible = 1;
                    p->timer = 0;
                    p->shotCount = 0;
                    p->state = VulcanCannon_UpdateFiringBurst;
                } else
                    p->targetVisible = 0;
            } else
                p->targetVisible = 0;
        } else {
            if (target.x > p->display.position.x - 0x80 &&
                target.x < p->display.position.x + 0x80 && target.z > p->display.position.z) {
                if (VulcanCannon_HasClearShot(p)) {
                    p->aimTimer = 0;
                    p->targetVisible = 1;
                    p->timer = 0;
                    p->shotCount = 0;
                    p->state = VulcanCannon_UpdateFiringBurst;
                } else
                    p->targetVisible = 0;
            } else
                p->targetVisible = 0;
        }
    } else
        p->targetVisible = 0;
    if (p->aimTimer == 119) {
        u8 angle;
        s32 facing;
        {
            struct SceneVec *targetPosition;
            struct ContactPosition *emitterPosition;
            targetPosition = Player_GetEffectivePosition(0);
            emitterPosition = &p->display.position;
            angle = Math_Atan2(targetPosition->x - emitterPosition->x,
                               targetPosition->z - emitterPosition->z);
        }
        p->timer = 0;
        p->shotCount = 0;
        facing = p->facing;
        if (facing == facingRight) {
            if ((u8)(angle - 0x21) <= 0x7e) {
                Anim_RestartSequence(&p->sequence, p->resource, 0x1c, 0, 8);
                p->timer = facing;
                p->state = VulcanCannon_UpdateFacingTurn;
            }
        } else {
            if ((u8)(angle - 0x21) > 0x7e) {
                Anim_RestartSequence(&p->sequence, p->resource, 0x1c, 0, 12);
                p->timer = 0;
                p->state = VulcanCannon_UpdateFacingTurn;
            }
        }
    }
    VulcanCannon_StepAnimation(p);
    return 0;
}
