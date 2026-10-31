#include "global.h"
#include "engine_globals.h"
#include "ram.h"
#include "scene_vec.h"
#include "player.h"
#include "contact_position.h"
#include "heap.h"
#include "resource_registry.h"
#include "monster_particle_random.h"

/* Additional fields used by the detached target-motion action (tentative names). */
struct PlayerTargetMotionView {
    u8 unknown0[0x10C];
    struct SceneVec position;
    u8 unknown114[0x12];
    u16 spriteResource;
    u8 unknown128[0xC];
    u32 variantResource;
    u8 unknown138[0x358];
    struct SceneVec motion;
    u8 unknown498[0x5F0];
    void *particles;
    struct SceneVec origin, destination;
    u16 duration, speed;
    u8 orbitAngle;
    u8 unknownAA1[0xF];
    u16 particlePalette;
};
#define MOTION ((struct PlayerTargetMotionView *)p)

struct MotionTargetBody {
    u8 unknown0[12];
    struct SceneVec position;
};
struct MotionTarget {
    struct MotionTargetBody *body;
    u32 flags;
};
static inline struct SceneVec *MotionTarget_GetPosition(struct MotionTarget *target)
{
    if (target->flags & 16)
        return &target->body->position;
    return &target->body->position;
}

extern u8 *gUnk_02000538;
extern u32 gUnk_0200051C;
extern u32 gUnk_02000524;
extern const s16 gUnk_086149C4[];

#define TGT_POS() ((s16 *)MotionTarget_GetPosition((struct MotionTarget *)gUnk_02000538))

static inline void PlayerMotion_SetTarget(void *target)
{
    gUnk_02000538 = target;
}
s32 Player_ConsumeStateEntry(void *);
void *ParticlePool_Create(void);
void *sub_0814B078(void *, s32, s32);
void Player_OnTargetMotionExit(void);
void Player_SetPendingCallback(void *, void *);
void WirelessGame_ResetCyclingItemLatch(void);
void Player_EmitQueuedEffects(void *);
void Player_NotifyActionCategories(void *);
void Player_AddControlFlags(void *, u32);
void Sound_PlaySong(u32);
void Player_SetActionPhase(void *, u32);
void Player_SetFacingFromAngle(void *, s32);
s32 Math_Atan2(s32, s32);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
void Anim_Step(void *, void *);
void ParticlePool_EmitTriple(void *, void *, s32, s32);
void SceneMovingParticle_Create(void *, void *, s32, s32);
void PlayerCue_InitAttackCollider(void *, void *, s32, s32);
s32 Div(s32, s32);
void TemporaryAnimationPool_CreateEntry(void *, s32, s32, s32, s32);
void Player_UpdateFacingSide(void *);
void ActorSequence_Begin(void *, s32, s32);

/* fixed-point multiply result, truncating toward zero */
static inline s32 PlayerMotion_Scale12(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}

static inline void PlayerMotion_SetPolar(struct ContactPosition *out, s32 angle, s32 magnitude)
{
    s32 rawX = gUnk_086149C4[(angle + 64) & 255] * magnitude;
    const s16 *sine = gUnk_086149C4;
    s32 x = PlayerMotion_Scale12(rawX);
    s32 zero = 0;
    out->x = x;
    out->y = zero;
    out->z = PlayerMotion_Scale12(sine[angle & 255] * magnitude);
}

static inline void PlayerMotion_ClearAnimationFlags(struct PlayerAnimation *animation, u32 flags)
{
    u32 value = animation->flags;
    value &= ~flags;
    animation->flags = value;
}

static inline void PlayerMotion_AddPosition(struct SceneVec *out, const struct SceneVec *a,
                                            const struct SceneVec *b)
{
    out->x = b->x + a->x;
    out->y = b->y + a->y;
    out->z = b->z + a->z;
}

#define PLAYER_START_ANIM(A0, A1, N, F)                                                            \
    do {                                                                                           \
        struct PlayerAnimation *animation;                                                         \
        void *data;                                                                                \
        if (p->heading == 0) {                                                                     \
            animation = &p->secondaryAnimation;                                                    \
            data = p->animationData;                                                               \
            Anim_SetKeyframes(animation, data, A0, N, F);                                          \
        } else {                                                                                   \
            animation = &p->secondaryAnimation;                                                    \
            data = p->animationData;                                                               \
            Anim_SetKeyframes(animation, data, A1, N, F);                                          \
        }                                                                                          \
        if (p->side)                                                                               \
            animation->flags |= 4;                                                                 \
        else                                                                                       \
            animation->flags &= ~4;                                                                \
    } while (0)

#define PLAYER_START_ANIM_OWNER(A0, A1, N, F)                                                      \
    do {                                                                                           \
        void *data;                                                                                \
        if (p->heading == 0) {                                                                     \
            animation = &p->secondaryAnimation;                                                    \
            data = p->animationData;                                                               \
            Anim_SetKeyframes(animation, data, A0, N, F);                                          \
        } else {                                                                                   \
            animation = &p->secondaryAnimation;                                                    \
            data = p->animationData;                                                               \
            Anim_SetKeyframes(animation, data, A1, N, F);                                          \
        }                                                                                          \
        if (p->side)                                                                               \
            animation->flags |= 4;                                                                 \
        else                                                                                       \
            animation->flags &= ~4;                                                                \
    } while (0)

static inline s32 PlayerMotion_RandomFrom(const u16 *table)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return table[gRandomIndex];
}

static inline s32 PlayerMotion_ReadRandomSample(void)
{
    u32 index = (gRandomIndex + 1) & 1023;
    gRandomIndex = index;
    return *(const u16 *)(RANDOM_TABLE_ADDR + index * 2);
}

#ifdef NONMATCHING
// fuzzy: 82.4%
// confidence=low: player "throw/bolt at target" state machine: 8-phase switch (approach, dash, strike, return)
#define Player_UpdateTargetMotion sub_0814B1F8
void Player_UpdateTargetMotion(struct Player *p)
{
    struct ContactPosition vel, particlePosition, vel2, particlePosition2, pos;

    if (Player_ConsumeStateEntry(p)) {
        if (MOTION->particles == 0)
            MOTION->particles = ParticlePool_Create();
        {
            u16 *speed = &MOTION->speed;
            u32 zero = 0;
            *speed = 50;
            PlayerMotion_SetTarget(sub_0814B078(p, 1000000, zero));
            gUnk_0200051C = zero;
            gUnk_02000524 = zero;
        }
        p->movementStart = p->position;
        MOTION->position = p->previousPosition;
        PlayerMotion_ClearAnimationFlags(&p->secondaryAnimation, 1);
        Player_SetPendingCallback(p, Player_OnTargetMotionExit);
        if (gEngineFlags & 0x1000)
            WirelessGame_ResetCyclingItemLatch();
        Player_EmitQueuedEffects(p);
        Player_NotifyActionCategories(p);
    }

    MOTION->spriteResource = p->spriteResource;
    MOTION->variantResource = p->actionVariantResource;
    Player_AddControlFlags(p, 9);
    p->statusFlags = (p->statusFlags & ~0x4000) | 0x60000;

    switch (p->tetherPhase) {
        case 0: {
            s32 dx, dy;
            s16 *tp;
            struct PlayerAnimation *animation;
            if (gUnk_02000538 == 0) {
                Sound_PlaySong(0x124);
                Player_SetActionPhase(p, 5);
                break;
            }
            animation = &p->secondaryAnimation;
            if (p->tetherFrame == 0) {
                tp = TGT_POS();
                dx = tp[0] - MOTION->position.x;
                tp = TGT_POS();
                dy = tp[2] - MOTION->position.z;
                Player_SetFacingFromAngle(p, Math_Atan2(dx, dy));
                if (p->heading == 0) {
                    animation = &p->secondaryAnimation;
                    Anim_SetKeyframes(animation, p->animationData, 0x1A1, 1, 0);
                } else {
                    animation = &p->secondaryAnimation;
                    Anim_SetKeyframes(animation, p->animationData, 0x1A6, 1, 0);
                }
                if (p->side)
                    animation->flags |= 4;
                else
                    animation->flags &= ~4;
            }
            Anim_Step(animation, p->animationData);
            p->tetherFrame++;
            if (p->tetherFrame <= 10)
                break;
            goto phase1_exit;
        }
        case 1: {
            s32 dx, dy, ang, cnt, q, r0, r1;
            s16 *tp;
            struct PlayerAnimation *animation;
            tp = TGT_POS();
            dx = tp[0] - MOTION->position.x;
            tp = TGT_POS();
            dy = tp[2] - MOTION->position.z;
            ang = Math_Atan2(dx, dy);
            ParticlePool_EmitTriple(MOTION->particles, &MOTION->position, (ang + 0x80) & 0xFF, 0);
            cnt = p->tetherFrame;
            animation = &p->secondaryAnimation;
            if (cnt == 0) {
                Player_SetFacingFromAngle(p, ang);
                Sound_PlaySong(0x4F5);
                if (p->heading == 0)
                    Anim_SetKeyframes(animation, p->animationData, 0x1A2, 1, 0);
                else
                    Anim_SetKeyframes(animation, p->animationData, 0x1A7, 1, 0);
                if (p->side)
                    animation->flags |= 4;
                else
                    animation->flags &= ~4;
            }
            Anim_Step(animation, p->animationData);
            if (dx * dx + dy * dy < MOTION->speed * MOTION->speed) {
                p->facing = (((ang + 0x20) & 0xFF) >> 6) * 2 + 3 & 7;
                MOTION->position.y = TGT_POS()[1];
                Player_SetFacingFromAngle(p, ang);
                Player_SetActionPhase(p, 2);
                {
                    const u16 *earlyTable = (const u16 *)RANDOM_TABLE_ADDR;
                    struct ContactPosition *velocity;
                    r0 = PlayerMotion_RandomFrom(earlyTable) & 0xFF;
                    {
                        s32 raw = PlayerMotion_RandomFrom(earlyTable);
                        s32 mask = 31;
                        raw &= mask;
                        velocity = &vel;
                        r1 = raw;
                    }
                    r1 -= 16;
                    {
                        s32 x = PlayerMotion_Scale12(gUnk_086149C4[(r0 + 0x40) & 0xFF] * r1);
                        s32 zero = 0;
                        velocity->x = x;
                        velocity->y = zero;
                        velocity->z = PlayerMotion_Scale12(gUnk_086149C4[r0] * r1);
                    }
                }
                vel.y = (PlayerMotion_ReadRandomSample() & 0x3F) * 2 - 0x30;
                {
                    particlePosition = *(struct ContactPosition *)TGT_POS();
                    particlePosition.y += 30;
                    SceneMovingParticle_Create(&particlePosition, &vel, 60, 1);
                }
            } else {
                MOTION->position.x +=
                    PlayerMotion_Scale12(gUnk_086149C4[(ang + 0x40) & 0xFF] * MOTION->speed);
                MOTION->position.z +=
                    PlayerMotion_Scale12(gUnk_086149C4[ang & 0xFF] * MOTION->speed);
                p->facing = (((ang + 0x20) & 0xFF) >> 6) * 2 + 3 & 7;
                if (Div(p->movementDuration, 2) < p->tetherFrame) {
                    MOTION->position.y -= 0x50;
                    if (MOTION->position.y < TGT_POS()[1])
                        MOTION->position.y = TGT_POS()[1];
                } else {
                    MOTION->position.y += 0x50;
                    if (MOTION->position.y > TGT_POS()[1] + 1000)
                        MOTION->position.y = TGT_POS()[1] + 1000;
                }
                q = ((ang + 0x60) & 0xFF) >> 6;
                if (q > 1) {
                    p->heading = 3 - q;
                    p->side = 1;
                } else {
                    p->heading = q;
                    p->side = 0;
                }
                p->tetherFrame++;
                if (p->tetherFrame > 0x59)
                    Player_SetActionPhase(p, 6);
            }
            if (p->inputKeys[1] & 2)
                gUnk_02000524 = 1;
            if (p->historyDirectionMode == 1 && (p->inputKeys[1] & 1))
                gUnk_02000524 = 1;
            PlayerCue_InitAttackCollider(p, &MOTION->position, 1, 0x60);
            break;
        }
        case 2: {
            s32 dx, dy, ang, r0, r1;
            s16 *tp;
            if (p->tetherFrame == 0) {
                Sound_PlaySong(0x4FB);
                tp = TGT_POS();
                dx = tp[0] - MOTION->position.x;
                tp = TGT_POS();
                dy = tp[2] - MOTION->position.z;
                if (dx * dx + dy * dy > 0x9C4) {
                    ang = Math_Atan2(dx, dy);
                    Player_SetFacingFromAngle(p, ang);
                    MOTION->position.x =
                        TGT_POS()[0] +
                        PlayerMotion_Scale12(gUnk_086149C4[(ang + 0x40) & 0xFF] * 0x32);
                    MOTION->position.z =
                        TGT_POS()[2] + PlayerMotion_Scale12(gUnk_086149C4[ang & 0xFF] * 0x32);
                }
                gUnk_0200051C = 0;
            }
            if (p->tetherFrame == 5)
                PLAYER_START_ANIM(0x1A3, 0x1A8, 2, 0);
            if ((p->tetherFrame & 1) == 0) {
                u32 randomOrigin = (u32)&gHeapHead + RESOURCE_REGISTRY_ENTRIES_ADDR;
                u32 sampleOrigin;
                struct ContactPosition *velocity;
                struct ContactPosition *result;
                const u16 *earlyTable = (const u16 *)RANDOM_TABLE_ADDR;
                r0 = PlayerMotion_RandomFrom(earlyTable) & 0xFF;
                r1 = PlayerMotion_RandomFrom(earlyTable) & 0x1F;
                velocity = &vel2;
                r1 -= 16;
                {
                    s32 rawX = gUnk_086149C4[(r0 + 0x40) & 0xFF] * r1;
                    sampleOrigin = randomOrigin;
                    result = velocity;
                    velocity->x = PlayerMotion_Scale12(rawX);
                }
                velocity->y = 0;
                velocity->z = PlayerMotion_Scale12(gUnk_086149C4[r0] * r1);
                result->y = (ParticleRandom_ReadRelativeSample(sampleOrigin,
                                                               (const u16 *)RANDOM_TABLE_ADDR) &
                             0x3F) *
                                2 -
                            0x30;
                {
                    particlePosition2 = *(struct ContactPosition *)TGT_POS();
                    particlePosition2.y += 30;
                    SceneMovingParticle_Create(&particlePosition2, result, 60, 1);
                }
            }
            p->tetherFrame++;
            if (p->tetherFrame > 0x19) {
                if (gUnk_0200051C != 0 && gUnk_02000524 == 0 && p->statusValue != 0) {
                    PlayerMotion_SetTarget(sub_0814B078(p, 1000000, 0));
                    if (gUnk_02000538 != 0) {
                        MOTION->speed += 0x32;
                        if (MOTION->speed > 0xC8)
                            MOTION->speed = 0xC8;
                    phase1_exit:
                        Player_SetActionPhase(p, 1);
                        break;
                    }
                }
                Player_SetActionPhase(p, 6);
                break;
            }
            if (p->inputKeys[1] & 2)
                gUnk_0200051C = 1;
            if (p->historyDirectionMode == 1 && (p->inputKeys[1] & 1))
                gUnk_0200051C = 1;
            break;
        }
        case 3: {
            struct PlayerAnimation *animation;
            s16 cnt;
            if (p->tetherFrame == 0) {
                MOTION->orbitAngle = (((u8)p->unk2D - 0x20) & ~0x3F) + 0x40;
                Sound_PlaySong(0x4FC);
                Sound_PlaySong(0x4FA);
            }
            cnt = p->tetherFrame;
            if ((u16)cnt > 5) {
                MOTION->orbitAngle += 0x12 + ((u16)cnt >> 1);
                {
                    s32 speed = p->tetherFrame + 0x4B;
                    PlayerMotion_SetPolar(&pos, MOTION->orbitAngle, speed * 2);
                }
            } else if ((u16)cnt == 5) {
                MOTION->orbitAngle += 0x40;
                PlayerMotion_SetPolar(&pos, MOTION->orbitAngle, (p->tetherFrame >> 1) + 0x14);
            } else {
                PlayerMotion_SetPolar(&pos, MOTION->orbitAngle, ((u16)cnt >> 1) + 0x28);
            }
            MOTION->position.x += pos.x;
            MOTION->position.z += pos.z;
            MOTION->position.x += MOTION->motion.x;
            MOTION->position.z += MOTION->motion.z;
            ParticlePool_EmitTriple(MOTION->particles, &MOTION->position,
                                    (MOTION->orbitAngle + 0xA0) & 0xFF, 0);
            Player_SetFacingFromAngle(p, MOTION->orbitAngle);
            PLAYER_START_ANIM_OWNER(0x1A2, 0x1A7, 1, 1);
            Anim_Step(animation, p->animationData);
            if ((p->tetherFrame & 3) == 0)
                TemporaryAnimationPool_CreateEntry(&p->secondaryAnimation, 0x10, 0x200,
                                                   MOTION->particlePalette, 0);
            PlayerCue_InitAttackCollider(p, &MOTION->position, 1, 0x20);
            p->tetherFrame++;
            if (p->tetherFrame > 0x1D)
                Player_SetActionPhase(p, 4);
            break;
        }
        case 4: {
            s32 dx, dy, ang;
            if (p->tetherFrame == 0) {
                MOTION->orbitAngle = (((u8)p->unk2D - 0x20) & ~0x3F) + 0x40;
                ang = (MOTION->orbitAngle - 0x80) & 0xFF;
                PlayerMotion_SetPolar((struct ContactPosition *)&MOTION->position, ang, 0x100);
                PlayerMotion_AddPosition(&MOTION->position, &MOTION->position, &p->position);
            }
            dx = p->position.x - MOTION->position.x;
            dy = p->position.z - MOTION->position.z;
            ang = Math_Atan2(dx, dy);
            if (dx * dx + dy * dy <= 0xE0F) {
                MOTION->position = p->position;
                Player_SetActionPhase(p, 7);
            } else {
                MOTION->position.x += PlayerMotion_Scale12(gUnk_086149C4[(ang + 0x40) & 0xFF] * 20);
                MOTION->position.z += PlayerMotion_Scale12(gUnk_086149C4[ang & 0xFF] * 20);
                p->tetherFrame++;
                if (p->tetherFrame > 0x59)
                    Player_SetActionPhase(p, 7);
            }
            if (p->tetherFrame == 0)
                PLAYER_START_ANIM(0x1A4, 0x1A9, 2, 0);
            goto step_and_count;
        }
        case 5: {
            struct PlayerAnimation *animation;
            PLAYER_START_ANIM_OWNER(0x1A1, 0x1A6, 1, 0);
            Anim_Step(animation, p->animationData);
            Player_UpdateFacingSide(p);
            p->tetherFrame++;
            if (p->tetherFrame > 0x1F)
                ActorSequence_Begin(p, 0, 0);
            MOTION->position = p->position;
            p->statusFlags |= 0x4000;
            break;
        }
        case 6: {
            s32 dx, dy, ang;
            dx = p->position.x - MOTION->position.x;
            dy = p->position.z - MOTION->position.z;
            ang = Math_Atan2(dx, dy);
            if (dx * dx + dy * dy <= 0xE0F) {
                MOTION->position = p->position;
                Player_SetActionPhase(p, 7);
            } else {
                MOTION->position.x +=
                    PlayerMotion_Scale12(gUnk_086149C4[(ang + 0x40) & 0xFF] * 0x32);
                MOTION->position.z += PlayerMotion_Scale12(gUnk_086149C4[ang & 0xFF] * 0x32);
                p->tetherFrame++;
                if (p->tetherFrame > 0x59)
                    Player_SetActionPhase(p, 7);
            }
            if (p->tetherFrame == 0)
                PLAYER_START_ANIM(0x1A4, 0x1A9, 2, 0);
        step_and_count:
            Anim_Step(&p->secondaryAnimation, p->animationData);
            p->tetherFrame++;
            break;
        }
        case 7: {
            struct PlayerAnimation *animation;
            animation = &p->secondaryAnimation;
            if (p->tetherFrame == 0)
                PLAYER_START_ANIM_OWNER(0x1A4, 0x1A9, 1, 0);
            Anim_Step(animation, p->animationData);
            if (p->tetherFrame > 10)
                ActorSequence_Begin(p, 0, 0);
            p->tetherFrame++;
            break;
        }
    }
}
#else
INCLUDE_ASM("asm/nonmatching", sub_0814B1F8);
#endif
