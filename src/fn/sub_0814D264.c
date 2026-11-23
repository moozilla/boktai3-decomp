#include "global.h"
#include "player.h"
#include "contact_collider.h"

// confidence=low: selects sweep, vertical-arc or radial reach from the active attack animation.
#define Player_UpdateAttackColliderMotion sub_0814D264
extern const s16 gUnk_086149C4[];
u32 ActorFlags_GetResponseBits(void *);
s32 ActorFlags_SelectResponse(void *);
s32 Player_CalculateActionStat(void *, void *, u32);
void PlayerAttackEffect_SetEmissionState(u32);
s32 Div(s32, s32);
void Sound_PlaySong(u32);
void Collider_SetPosition(void *, void *, u32);
void Collider_Refresh(void *);
void Collider_SetAttackProperties(void *, u32, u32, u32, u32, u32);
s32 Player_ComputeActionScale(void *, void *);
void Collider_SetContactMode(void *, s32);
static inline s32 Scale12(s32 value)
{
    return value >= 0 ? value >> 12 : -((-value) >> 12);
}
void Player_UpdateAttackColliderMotion(struct Player *p, s32 forced)
{
    struct PlayerStatModifierContext *q = &p->statModifierContext;
    u32 flags = ActorFlags_GetResponseBits(p);
    s32 power;
    s32 animation;
    s32 radialRadius;
    if (p->currentStatB < ActorFlags_SelectResponse(p))
        flags = 0;
    if (forced) {
        flags |= 0x40000;
        p->statModifierContext.actionTimer = 100;
    } else
        p->statModifierContext.actionTimer = 20;
    power = Player_CalculateActionStat(p, &p->statModifierContext, p->derivedStats[2]);
    animation = p->animationIndex;
    switch (animation) {
        case 0x54: {
            s32 angle;
            PlayerAttackEffect_SetEmissionState(1);
            q->motionSpeed = Div(512, p->animation.durationScale);
            q->swingFrames = 8;
            if (((u16)(p->animation.frameIndex - 1)) > 1) {
                p->statModifierContext.previousAnimation = -1;
                return;
            }
            if (p->statModifierContext.previousAnimation != 0x54)
                Sound_PlaySong(0x38C);
            p->statModifierContext.previousAnimation = animation;
            ++q->contactTimer;
            if (p->side) {
                u8 *heading = &q->contactHeading;
                angle = ((*heading) - (q->contactTimer * q->motionSpeed)) + 0x120;
                angle &= 255;
            } else {
                u8 *heading = &q->contactHeading;
                s32 sum = (q->contactTimer * q->motionSpeed) + 0x100;
                sum += *heading;
                angle = sum - 0x20;
                angle &= 255;
            }
            q->contactDestination.x =
                q->contactOrigin.x + Scale12(gUnk_086149C4[(angle + 64) & 255] * 160);
            q->contactDestination.y = q->contactOrigin.y;
            q->contactDestination.z = q->contactOrigin.z + Scale12(gUnk_086149C4[angle] * 160);
            break;
        }

        case 0x81: {
            s32 angle;
            s32 radius;
            PlayerAttackEffect_SetEmissionState(2);
            q->motionSpeed = Div(1216, p->animation.durationScale);
            if (p->animation.frameIndex <= 1) {
                p->statModifierContext.previousAnimation = -1;
                return;
            }
            if (p->animation.frameIndex > 3)
                break;
            if (p->statModifierContext.previousAnimation != 0x81)
                Sound_PlaySong(0x386);
            p->statModifierContext.previousAnimation = animation;
            ++q->contactTimer;
            angle = (0x150 - (q->contactTimer * q->motionSpeed)) & 255;
            q->contactDestination.y =
                q->contactOrigin.y + Scale12(gUnk_086149C4[angle & 255] * 250);
            radius = Scale12(gUnk_086149C4[(angle + 64) & 255] * 250);
            q->contactDestination.x =
                q->contactOrigin.x +
                Scale12(gUnk_086149C4[(q->contactHeading + 64) & 255] * radius);
            q->contactDestination.z =
                q->contactOrigin.z + Scale12(radius * gUnk_086149C4[q->contactHeading]);
            break;
        }

        case 0x72:
            PlayerAttackEffect_SetEmissionState(3);
            q->motionSpeed = Div(3584, p->animation.durationScale);
            q->swingFrames = 0;
            q->holdFrames = 3;
            ++q->contactTimer;
            if (q->contactTimer < q->swingFrames) {
                p->statModifierContext.previousAnimation = -1;
                radialRadius = (q->motionSpeed * q->contactTimer) + 0xCC;
                q->contactDestination.x =
                    q->contactOrigin.x +
                    Scale12(gUnk_086149C4[(q->contactHeading + 64) & 255] * radialRadius);
                q->contactDestination.y = q->contactOrigin.y;
                q->contactDestination.z =
                    q->contactOrigin.z + Scale12(gUnk_086149C4[q->contactHeading] * radialRadius);
            } else if (q->contactTimer < (q->swingFrames + q->holdFrames)) {
                if (p->statModifierContext.previousAnimation != 0x72)
                    Sound_PlaySong(0x385);
                p->statModifierContext.previousAnimation = animation;
                radialRadius = (q->motionSpeed * q->swingFrames) + 0xCC;
                q->contactDestination.x =
                    q->contactOrigin.x +
                    Scale12(gUnk_086149C4[(q->contactHeading + 64) & 255] * radialRadius);
                q->contactDestination.y = q->contactOrigin.y;
                q->contactDestination.z =
                    q->contactOrigin.z + Scale12(gUnk_086149C4[q->contactHeading] * radialRadius);
            }
            break;

        default:
            p->statModifierContext.previousAnimation = -1;
            break;
    }

    Collider_SetPosition(&q->attackCollider, &q->contactDestination, 0);
    Collider_Refresh(&q->attackCollider);
    Collider_SetAttackProperties(&q->attackCollider, power, q->actionTimer, 1, flags,
                                 p->statModifierContext.contactParameter);
    Collider_SetContactMode(&p->statModifierContext.attackCollider,
                            Player_ComputeActionScale(p, &q->attackCollider));
    if (q->thresholdSatisfied == 1)
        ContactCollider_SetReactionFlags(&q->attackCollider, 0x800000);
    {
        u8 heading = q->contactHeading;
        q->attackCollider.direction = heading;
    }
}
