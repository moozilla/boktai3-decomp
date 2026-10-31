#include "global.h"
#include "engine.h"
#include "ram.h"
#include "player.h"
#include "contact_position.h"
#include "monster_particle_random.h"
#include "heap.h"
#include "resource_registry.h"

extern const s16 gUnk_086149C4[];
extern u32 gUnk_0200051C, gUnk_02000524;

s32 Player_ConsumeStateEntry(void *);
u32 ParticlePool_Create(void);
void Player_SetPendingCallback(void *, void *);
void Player_OnTargetMotionExit(void);
void Player_EmitQueuedEffects(void *);
void Player_NotifyActionCategories(void *);
void Player_AddControlFlags(void *, u32);
s32 Math_Atan2(s32, s32);
void Player_SetFacingFromAngle(void *, s32);
void Anim_SetKeyframes(void *, void *, u32, u32, u32);
s32 Anim_Step(void *, void *);
void Player_SetActionPhase(void *, s32);
void ParticlePool_EmitTriple(u32, void *, u32, u32);
void Sound_PlaySong(u32);
void SceneMovingParticle_Create(void *, void *, s32, s32);
void PlayerCue_InitAttackCollider(void *, void *, u32, u32);
s32 Div(s32, s32);
void ActorSequence_Begin(void *, s32, s32);
void Player_UpdateFacingSide(void *);
void PlayerCue_EnterStandUpdate(void *);
void PlayerCue_CompleteAction(void *);

struct PlayerTargetDash {
    u8 unknown0[0x20];
    u32 statusFlags;
    u8 unknown24[0xC];
    struct ContactPosition position;
    u8 unknown38[0x34];
    u8 animationData[0x20];
    u8 unknown8C[0x20];
    struct ContactPosition previousPosition;
    u8 unknownB4[0x12];
    u16 spriteResource;
    u8 unknownC8[0xC];
    u32 actionVariantResource;
    u8 unknownD8[0x14];
    struct PlayerAnimation secondaryAnimation;
    u8 unknownF8[0x14];
    struct ContactPosition alternatePosition;
    u8 unknown114[0x12];
    u16 secondarySpriteResource;
    u8 unknown128[0xC];
    u32 secondaryVariantResource;
    u8 unknown138[0x210];
    const u16 *inputKeys;
    u8 unknown34C[0x56];
    u8 heading, side, facing;
    u8 unknown3A5[0xB3];
    u8 phase, unknown459;
    u16 timer;
    u8 unknown45C[0x130];
    void *pendingCallback;
    struct ContactPosition scriptDestination;
    u8 unknown598[0x4F0];
    u32 particlePool;
    struct ContactPosition movementStart, movementDestination;
    u16 movementDuration, speed;
};
static inline void ClearAnimationFlags(struct PlayerAnimation *animation, u32 mask)
{
    u32 flags = animation->flags;
    flags &= ~mask;
    animation->flags = flags;
}
static inline u32 ClearStatusFlags(u32 *flags, u32 clear)
{
    return *flags & ~clear;
}
static inline s32 TargetDash_ScaleMotion(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}

static inline u32 TargetDash_ReadParticleRandom(void)
{
    return ParticleRandom_ReadDirectSample(gRandomTable);
}

union TargetDashKeyframeSelection {
    u32 raw;
    u16 sequence;
};
static inline void TargetDash_SetKeyframes(u32 index, void *anim, void *data, u32 mode, u32 flags)
{
    union TargetDashKeyframeSelection selection;
    selection.raw = index;
    Anim_SetKeyframes(anim, data, selection.sequence, mode, flags);
}
static inline void TargetDash_StartAnimation(struct PlayerTargetDash *p,
                                             struct PlayerAnimation *anim, void *data, u32 a, u32 b,
                                             u32 mode)
{
    if (p->heading == 0)
        TargetDash_SetKeyframes(a, anim, data, mode, 0);
    else
        TargetDash_SetKeyframes(b, anim, data, mode, 0);
    if (p->side)
        anim->flags |= 4;
    else
        anim->flags &= ~4;
}

static inline void TargetDash_EmitTargetSpark(struct PlayerTargetDash *p)
{
    struct ContactPosition vel;
    struct ContactPosition pos;
    struct ContactPosition *v;
    struct ContactPosition *q;
    struct ContactPosition *verticalResult;
    s32 vertical;
    u32 a = TargetDash_ReadParticleRandom() & 0xFF;
    s32 raw = TargetDash_ReadParticleRandom() & 0x1F;
    s32 m;
    v = &vel;
    m = raw - 0x10;
    v->x = TargetDash_ScaleMotion(gUnk_086149C4[(a + 0x40) & 0xFF] * m);
    v->y = 0;
    v->z = TargetDash_ScaleMotion(gUnk_086149C4[a] * m);
    vertical =
        ((ParticleRandom_ReadDirectSample((const u16 *)RANDOM_TABLE_ADDR) & 0x3F) << 1) - 0x30;
    verticalResult = &vel;
    verticalResult->y = vertical;
    q = &pos;
    *q = p->scriptDestination;
    q->y += 0x1E;
    SceneMovingParticle_Create(&pos, &vel, 60, 1);
}

static inline void TargetDash_EmitRelativeTargetSpark(struct PlayerTargetDash *p)
{
    struct ContactPosition vel;
    struct ContactPosition pos;
    struct ContactPosition *v;
    struct ContactPosition *q;
    struct ContactPosition *verticalResult;
    s32 vertical;
    u32 randomOrigin = (u32)&gHeapHead + RESOURCE_REGISTRY_ENTRIES_ADDR;
    const u16 *earlyTable = (const u16 *)(randomOrigin - (u32)&gHeapHead +
                                          (RANDOM_TABLE_ADDR - RESOURCE_REGISTRY_ENTRIES_ADDR));
    u32 sampleOrigin;
    u32 a = ParticleRandom_ReadDirectSample(earlyTable) & 0xFF;
    s32 raw = ParticleRandom_ReadDirectSample(earlyTable) & 0x1F;
    s32 m;
    v = &vel;
    m = raw - 0x10;
    {
        s32 rawX = gUnk_086149C4[(a + 0x40) & 0xFF] * m;
        sampleOrigin = randomOrigin;
        v->x = TargetDash_ScaleMotion(rawX);
    }
    v->y = 0;
    v->z = TargetDash_ScaleMotion(gUnk_086149C4[a] * m);
    vertical =
        ((ParticleRandom_ReadRelativeSample(sampleOrigin, (const u16 *)RANDOM_TABLE_ADDR) & 0x3F)
         << 1) -
        0x30;
    verticalResult = &vel;
    verticalResult->y = vertical;
    q = &pos;
    *q = p->scriptDestination;
    q->y += 0x1E;
    SceneMovingParticle_Create(&pos, &vel, 60, 1);
}

// confidence=low: Player action "dash to target": 5-phase state machine (approach, run, strike, return, stand)
void PlayerCue_UpdateTargetDash(struct PlayerTargetDash *p)
{
    struct PlayerAnimation *anim;
    void *data;
    u32 animationFrame;

    if (Player_ConsumeStateEntry(p)) {
        if (p->particlePool == 0)
            p->particlePool = ParticlePool_Create();
        p->speed = 0x32;
        gUnk_0200051C = 0;
        gUnk_02000524 = 0;
        p->movementStart = p->position;
        p->alternatePosition = p->previousPosition;
        ClearAnimationFlags(&p->secondaryAnimation, 1);
        Player_SetPendingCallback(p, Player_OnTargetMotionExit);
        Player_EmitQueuedEffects(p);
        Player_NotifyActionCategories(p);
    }
    p->secondarySpriteResource = p->spriteResource;
    p->secondaryVariantResource = p->actionVariantResource;
    Player_AddControlFlags(p, 9);
    p->statusFlags = ClearStatusFlags(&p->statusFlags, 0x4000) | 0x60000;

    switch (p->phase) {
        case 0: {
            u32 frame;
            frame = p->timer;
            anim = &p->secondaryAnimation;
            data = p->animationData;
            if (frame == 0) {
                Player_SetFacingFromAngle(
                    p, Math_Atan2(p->scriptDestination.x - p->alternatePosition.x,
                                  p->scriptDestination.z - p->alternatePosition.z));
                TargetDash_StartAnimation(p, anim, data, 0x1A1, 0x1A6, 1);
            }
            Anim_Step(anim, data);
            p->timer++;
            if (p->timer > 10)
                Player_SetActionPhase(p, 1);
            break;
        }
        case 1: {
            s32 dx, dy, ang, ph;
            u32 frame;
            dx = p->scriptDestination.x - p->alternatePosition.x;
            dy = p->scriptDestination.z - p->alternatePosition.z;
            ang = Math_Atan2(dx, dy);
            ParticlePool_EmitTriple(p->particlePool, &p->alternatePosition, (ang + 0x80) & 0xFF, 0);
            frame = p->timer;
            anim = &p->secondaryAnimation;
            data = p->animationData;
            if (frame == 0) {
                Player_SetFacingFromAngle(p, ang);
                Sound_PlaySong(0x4F5);
                TargetDash_StartAnimation(p, anim, data, 0x1A2, 0x1A7, 1);
            }
            Anim_Step(anim, data);
            if (dx * dx + dy * dy < p->speed * p->speed) {
                p->facing = (((((ang + 0x20) & 0xFF) >> 6) << 1) + 3) & 7;
                p->alternatePosition.y = p->scriptDestination.y;
                Player_SetFacingFromAngle(p, ang);
                Player_SetActionPhase(p, 2);
                TargetDash_EmitTargetSpark(p);
            } else {
                p->alternatePosition.x +=
                    TargetDash_ScaleMotion(gUnk_086149C4[(ang + 0x40) & 0xFF] * p->speed);
                {
                    u32 speed = p->speed;
                    p->alternatePosition.z +=
                        TargetDash_ScaleMotion(gUnk_086149C4[ang & 0xFF] * speed);
                }
                p->facing = (((((ang + 0x20) & 0xFF) >> 6) << 1) + 3) & 7;
                if (Div(p->movementDuration, 2) < (s32)p->timer) {
                    p->alternatePosition.y -= 0x50;
                    if (p->alternatePosition.y < p->scriptDestination.y)
                        p->alternatePosition.y = p->scriptDestination.y;
                } else {
                    p->alternatePosition.y += 0x50;
                    if (p->alternatePosition.y > p->scriptDestination.y + 1000)
                        p->alternatePosition.y = p->scriptDestination.y + 1000;
                }
                ph = ((ang + 0x60) & 0xFF) >> 6;
                if (ph > 1) {
                    p->heading = 3 - ph;
                    p->side = 1;
                } else {
                    p->heading = ph;
                    p->side = 0;
                }
                p->timer++;
                if (p->timer > 0x59)
                    Player_SetActionPhase(p, 3);
            }
            if (p->inputKeys[1] & 2)
                gUnk_02000524 = 1;
            PlayerCue_InitAttackCollider(p, &p->alternatePosition, 1, 0x20);
            break;
        }
        case 2: {
            s32 dx, dy, ang;
            if (p->timer == 0) {
                Sound_PlaySong(0x4FB);
                dx = p->scriptDestination.x - p->alternatePosition.x;
                dy = p->scriptDestination.z - p->alternatePosition.z;
                if (dx * dx + dy * dy > 0x9C4) {
                    ang = Math_Atan2(dx, dy);
                    Player_SetFacingFromAngle(p, ang);
                    p->alternatePosition.x =
                        p->scriptDestination.x +
                        TargetDash_ScaleMotion(gUnk_086149C4[(ang + 0x40) & 0xFF] * 0x32);
                    p->alternatePosition.z =
                        p->scriptDestination.z +
                        TargetDash_ScaleMotion(gUnk_086149C4[ang & 0xFF] * 0x32);
                }
                gUnk_0200051C = 0;
            }
            if (p->timer == 5) {
                struct PlayerAnimation *animation;
                if (p->heading == 0) {
                    animation = &p->secondaryAnimation;
                    Anim_SetKeyframes(animation, p->animationData, 0x1A3, 2, 0);
                } else {
                    animation = &p->secondaryAnimation;
                    Anim_SetKeyframes(animation, p->animationData, 0x1A8, 2, 0);
                }
                if (p->side != 0)
                    animation->flags |= 4;
                else
                    animation->flags &= ~4;
            }
            if ((p->timer & 1) == 0)
                TargetDash_EmitRelativeTargetSpark(p);
            p->timer++;
            if (p->timer > 0x19) {
                Player_SetActionPhase(p, 3);
                break;
            }
            if (p->inputKeys[1] & 2)
                gUnk_0200051C = 1;
            break;
        }
        case 3: {
            s32 dx, dy, ang;
            dx = p->position.x - p->alternatePosition.x;
            dy = p->position.z - p->alternatePosition.z;
            ang = Math_Atan2(dx, dy);
            if (dx * dx + dy * dy <= 0xE0F) {
                p->alternatePosition = p->position;
                Player_SetActionPhase(p, 4);
            } else {
                p->alternatePosition.x +=
                    TargetDash_ScaleMotion(gUnk_086149C4[(ang + 0x40) & 0xFF] * 0x32);
                p->alternatePosition.z += TargetDash_ScaleMotion(gUnk_086149C4[ang & 0xFF] * 0x32);
                p->timer++;
                if (p->timer > 0x59)
                    Player_SetActionPhase(p, 4);
            }
            animationFrame = p->timer;
            anim = &p->secondaryAnimation;
            data = p->animationData;
            if (animationFrame == 0)
                TargetDash_StartAnimation(p, anim, data, 0x1A4, 0x1A9, 2);
            Anim_Step(anim, data);
            p->timer++;
            break;
        }
        case 4:
            animationFrame = p->timer;
            anim = &p->secondaryAnimation;
            data = p->animationData;
            if (animationFrame == 0)
                TargetDash_StartAnimation(p, anim, data, 0x1A4, 0x1A9, 1);
            Anim_Step(anim, data);
            if (p->timer > 10) {
                ActorSequence_Begin(p, 0, 0);
                Player_UpdateFacingSide(p);
                PlayerCue_EnterStandUpdate(p);
                PlayerCue_CompleteAction(p);
            }
            p->timer++;
            break;
    }
}
