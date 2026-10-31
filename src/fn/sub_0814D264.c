#include "global.h"
#include "player.h"
#include "contact_collider.h"

// confidence=low: selects sweep, vertical-arc or radial reach from the active attack animation.
#define Player_UpdateAttackColliderMotion sub_0814D264
struct AttackMotionContext {
    u8 unknown0[0x48];
    struct ContactCollider collider;
    struct SceneVec dest, origin;
    u8 unknownAC[0xC];
    u16 attackParameter;
    u8 unknownBA[2];
    u16 speed;
    u8 unknownBE[2];
    u16 phase1, phase2, unknownC4, timer;
    u8 heading;
    u8 unknownC9[0xC];
    u8 extendedFlags;
};
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
static inline void EnableExtendedContact(struct ContactCollider *collider)
{
    u32 mask = 0x800000;
    collider->reactionFlags |= mask;
}
struct AttackMotionPlayerFields {
    u8 unknown0[0x9C];
    u16 animationDuration;
    u8 unknown9E[0xB8C - 0x9E];
    struct ContactCollider collider;
    u8 unknownBE0[0xC1A - 0xBE0];
    s16 previousAnimation;
};
void Player_UpdateAttackColliderMotion(struct Player *p, s32 forced)
{
    struct AttackMotionContext *q = (struct AttackMotionContext *)&p->statModifierContext;
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
            q->speed = Div(512, ((struct AttackMotionPlayerFields *)p)->animationDuration);
            q->phase1 = 8;
            if (((u16)(p->actionCueFlags - 1)) > 1) {
                ((struct AttackMotionPlayerFields *)p)->previousAnimation = -1;
                return;
            }
            if (((struct AttackMotionPlayerFields *)p)->previousAnimation != 0x54)
                Sound_PlaySong(0x38C);
            ((struct AttackMotionPlayerFields *)p)->previousAnimation = animation;
            ++q->timer;
            if (p->side) {
                u8 *heading = &q->heading;
                angle = ((*heading) - (q->timer * q->speed)) + 0x120;
                angle &= 255;
            } else {
                u8 *heading = &q->heading;
                s32 sum = (q->timer * q->speed) + 0x100;
                sum += *heading;
                angle = sum - 0x20;
                angle &= 255;
            }
            q->dest.x = q->origin.x + Scale12(gUnk_086149C4[(angle + 64) & 255] * 160);
            q->dest.y = q->origin.y;
            q->dest.z = q->origin.z + Scale12(gUnk_086149C4[angle] * 160);
            break;
        }

        case 0x81: {
            s32 angle;
            s32 radius;
            PlayerAttackEffect_SetEmissionState(2);
            q->speed = Div(1216, ((struct AttackMotionPlayerFields *)p)->animationDuration);
            if (p->actionCueFlags <= 1) {
                ((struct AttackMotionPlayerFields *)p)->previousAnimation = -1;
                return;
            }
            if (p->actionCueFlags > 3)
                break;
            if (((struct AttackMotionPlayerFields *)p)->previousAnimation != 0x81)
                Sound_PlaySong(0x386);
            ((struct AttackMotionPlayerFields *)p)->previousAnimation = animation;
            ++q->timer;
            angle = (0x150 - (q->timer * q->speed)) & 255;
            q->dest.y = q->origin.y + Scale12(gUnk_086149C4[angle & 255] * 250);
            radius = Scale12(gUnk_086149C4[(angle + 64) & 255] * 250);
            q->dest.x = q->origin.x + Scale12(gUnk_086149C4[(q->heading + 64) & 255] * radius);
            q->dest.z = q->origin.z + Scale12(radius * gUnk_086149C4[q->heading]);
            break;
        }

        case 0x72:
            PlayerAttackEffect_SetEmissionState(3);
            q->speed = Div(3584, ((struct AttackMotionPlayerFields *)p)->animationDuration);
            q->phase1 = 0;
            q->phase2 = 3;
            ++q->timer;
            if (q->timer < q->phase1) {
                ((struct AttackMotionPlayerFields *)p)->previousAnimation = -1;
                radialRadius = (q->speed * q->timer) + 0xCC;
                q->dest.x =
                    q->origin.x + Scale12(gUnk_086149C4[(q->heading + 64) & 255] * radialRadius);
                q->dest.y = q->origin.y;
                q->dest.z = q->origin.z + Scale12(gUnk_086149C4[q->heading] * radialRadius);
            } else if (q->timer < (q->phase1 + q->phase2)) {
                if (((struct AttackMotionPlayerFields *)p)->previousAnimation != 0x72)
                    Sound_PlaySong(0x385);
                ((struct AttackMotionPlayerFields *)p)->previousAnimation = animation;
                radialRadius = (q->speed * q->phase1) + 0xCC;
                q->dest.x =
                    q->origin.x + Scale12(gUnk_086149C4[(q->heading + 64) & 255] * radialRadius);
                q->dest.y = q->origin.y;
                q->dest.z = q->origin.z + Scale12(gUnk_086149C4[q->heading] * radialRadius);
            }
            break;

        default:
            ((struct AttackMotionPlayerFields *)p)->previousAnimation = -1;
            break;
    }

    Collider_SetPosition(&q->collider, &q->dest, 0);
    Collider_Refresh(&q->collider);
    Collider_SetAttackProperties(&q->collider, power, q->attackParameter, 1, flags,
                                 p->statModifierContext.contactParameter);
    Collider_SetContactMode(&((struct AttackMotionPlayerFields *)p)->collider,
                            Player_ComputeActionScale(p, &q->collider));
    if (q->extendedFlags == 1)
        EnableExtendedContact((struct ContactCollider *)&q->collider);
    {
        u8 heading = q->heading;
        q->collider.direction = heading;
    }
}
