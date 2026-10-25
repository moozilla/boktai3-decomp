#include "global.h"
#include "player.h"

#ifdef NONMATCHING
// fuzzy: 71.7%
// fix.py best (sweep after the inlined-insn fix)
// Remaining differences: shared geometry tails, trigonometric table lifetimes and field scheduling.
// MATCHING_CONCESSION: register-pinning
// confidence=low: selects sweep, vertical-arc or radial reach from the active attack animation.
#define Player_UpdateAttackColliderMotion sub_0814D264
#define PH(n)                             (*(u16 *)((u8 *)p + (n)))
#define PS(n)                             (*(s16 *)((u8 *)p + (n)))
struct AttackMotionContext {
    u8 unknown0[0x48];
    u8 collider[0x54];
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
static inline void PlaceRadial(struct AttackMotionContext *q, s32 radius)
{
    q->dest.x = q->origin.x + Scale12(gUnk_086149C4[(q->heading + 64) & 255] * radius);
    q->dest.y = q->origin.y;
    q->dest.z = q->origin.z + Scale12(gUnk_086149C4[q->heading] * radius);
}
void Player_UpdateAttackColliderMotion(struct Player *p, s32 forced)
{
    struct AttackMotionContext *q = (void *)(((u8 *)p) + 0xB44);
    u32 flags = ActorFlags_GetResponseBits(p);
    s32 power;
    s32 animation;
    if ((*((u16 *)(((u8 *)p) + 0x428))) < ActorFlags_SelectResponse(p))
        flags = 0;
    if (forced) {
        flags |= 0x40000;
        *((u16 *)(((u8 *)p) + 0xBFC)) = 100;
    } else
        *((u16 *)(((u8 *)p) + 0xBFC)) = 20;
    power = Player_CalculateActionStat(p, q, p->derivedStats[2]);
    animation = p->animationIndex;
    switch (animation) {
        case 0x54: {
            s32 angle;
            PlayerAttackEffect_SetEmissionState(1);
            q->speed = Div(512, *((u16 *)(((u8 *)p) + 0x9C)));
            q->phase1 = 8;
            if (((u16)(p->actionCueFlags - 1)) > 1) {
                *((u16 *)(((u8 *)p) + 0xC1A)) = 0xFFFF;
                return;
            }
            if ((*((s16 *)(((u8 *)p) + 0xC1A))) != 0x54)
                Sound_PlaySong(0x38C);
            *((u16 *)(((u8 *)p) + 0xC1A)) = animation;
            ++q->timer;
            if (p->side) {
                u8 *heading = &q->heading;
                angle = ((*heading) - (q->timer * q->speed)) + 0x120;
                angle &= 255;
                asm("" : "+r"(angle));
            } else {
                u8 *heading = &q->heading;
                angle = (q->timer * q->speed) + 0x100;
                angle += *heading;
                angle -= 0x20;
                angle &= 255;
                asm("" : "+r"(angle));
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
            q->speed = Div(1216, *((u16 *)(((u8 *)p) + 0x9C)));
            if (p->actionCueFlags <= 1) {
                *((u16 *)(((u8 *)p) + 0xC1A)) = 0xFFFF;
                return;
            }
            if (p->actionCueFlags > 3)
                break;
            if ((*((s16 *)(((u8 *)p) + 0xC1A))) != 0x81)
                Sound_PlaySong(0x386);
            *((u16 *)(((u8 *)p) + 0xC1A)) = animation;
            ++q->timer;
            angle = (0x150 - (q->timer * q->speed)) & 255;
            q->dest.y = q->origin.y + Scale12(gUnk_086149C4[angle & 255] * 250);
            radius = Scale12(gUnk_086149C4[(angle + 64) & 255] * 250);
            q->dest.x = q->origin.x + Scale12(gUnk_086149C4[(q->heading + 64) & 255] * radius);
            q->dest.z = q->origin.z + Scale12(gUnk_086149C4[q->heading] * radius);
            break;
        }

        case 0x72:
            PlayerAttackEffect_SetEmissionState(3);
            q->speed = Div(3584, *((u16 *)(((u8 *)p) + 0x9C)));
            q->phase1 = 0;
            q->phase2 = 3;
            ++q->timer;
            if (q->timer < q->phase1) {
                *((u16 *)(((u8 *)p) + 0xC1A)) = 0xFFFF;
                PlaceRadial(q, (q->speed * q->timer) + 0xCC);
            } else if (q->timer < (q->phase1 + q->phase2)) {
                if ((*((s16 *)(((u8 *)p) + 0xC1A))) != 0x72)
                    Sound_PlaySong(0x385);
                *((u16 *)(((u8 *)p) + 0xC1A)) = animation;
                PlaceRadial(q, (q->speed * q->phase1) + 0xCC);
            }
            break;

        default:
            *((u16 *)(((u8 *)p) + 0xC1A)) = 0xFFFF;
            break;
    }

    Collider_SetPosition(q->collider, &q->dest, 0);
    Collider_Refresh(q->collider);
    Collider_SetAttackProperties(q->collider, power, q->attackParameter, 1, flags,
                                 *((u16 *)(((u8 *)p) + 0xBFE)));
    do {
        Collider_SetContactMode(((u8 *)p) + 0xB8C, Player_ComputeActionScale(p, q->collider));
    } while (0);
    if (q->extendedFlags == 1)
        *((u32 *)(((u8 *)q->collider) + 0x38)) |= 0x800000;
    *(((u8 *)q) + 0x8A) = q->heading;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_0814D264);
#endif
