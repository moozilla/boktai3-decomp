#ifndef GUARD_VANARGAND_PARTS_H
#define GUARD_VANARGAND_PARTS_H

#include "engine.h"
#include "paired_scene_actor.h"
#include "player_cue_sequence.h"
#include "vanargand_effect.h"
#include "scene_actor_terrain.h"

struct ContactPosition *Player_GetEffectivePosition(u32);
void MotionTrack_Update(void *);
void OrbitMotion_BeginDistanceWave(void *, const void *, const void *, s32, s32);
void Vanargand_EmitPartTrail(void *);
void AnimatedParticles_EmitBurst(void *, s32, s32, s32, s32, s32, s32, s32);
void ActorStatus_Deactivate(void *, void *);
void Palette_InterpolateBanks(void *, const void *, const void *, s32, s32);
void OrbitingParticles_Create(void *, int, int, int, int);
void ScaledShadow_Destroy(void *);
void SceneLink_RemoveNode(void *);
void Vanargand_SpawnPartEmergenceEffects(void *);

void PairedSceneActor_UpdateHalves(struct VanargandAttackPair *);
void Vanargand_DestroyPartController(struct VanargandAttackPair *);

static inline void Vanargand_Difference(struct ContactPosition *dst, struct ContactPosition *a,
                                        struct ContactPosition *b)
{
    dst->x = a->x - b->x;
    dst->y = a->y - b->y;
    dst->z = a->z - b->z;
}

static inline void Vanargand_SetState(struct PairedSceneHalf *p, u32 state)
{
    p->actionCounters[0] = state;
    p->actionCounters[1] = 0;
}

static inline s32 Vanargand_TruncateShift(s32 x, u32 bits)
{
    return x >= 0 ? x >> bits : -((-x) >> bits);
}

static inline void Vanargand_MakeVector(struct ContactPosition *out, s32 angle, s32 scale)
{
    out->x = Vanargand_TruncateShift(gUnk_086149C4[(angle + 64) & 255] * scale, 12);
    out->y = 0;
    out->z = Vanargand_TruncateShift(gUnk_086149C4[angle & 255] * scale, 12);
}

static inline void Vanargand_AddVector(struct ContactPosition *dst, struct ContactPosition *a,
                                       struct ContactPosition *b)
{
    dst->x = a->x + b->x;
    dst->y = a->y + b->y;
    dst->z = a->z + b->z;
}

static inline void Vanargand_SetFlags(struct PairedSceneHalf *p, u32 mask)
{
    p->actionFlags |= mask;
}

static inline u32 Vanargand_ReadEntryFlag(u8 *flag, u8 **saved)
{
    u32 value = *flag;
    *saved = flag;
    return value;
}

static inline u8 Vanargand_MotionEnded(struct PairedSceneHalf *p)
{
    return p->motion.active == 2;
}

static inline s32 Vanargand_TruncateScale(s32 x)
{
    return x >= 0 ? x >> 12 : -((-x) >> 12);
}

static inline void Vanargand_RetreatVector(struct ContactPosition *out, s32 angle)
{
    s32 scale = 720;
    s32 x = gUnk_086149C4[(angle + 64) & 255] * scale;
    out->x = Vanargand_TruncateScale(x);
    out->y = 0;
    x = gUnk_086149C4[angle & 255] * scale;
    out->z = Vanargand_TruncateScale(x);
}

static inline s32 Vanargand_AimAngle(struct PairedSceneHalf *p)
{
    s32 x = p->sprite.position.x, z = p->sprite.position.z;
    s32 dx = Player_GetEffectivePosition(0)->x;
    s32 angle;
    {
        s32 dz = Player_GetEffectivePosition(0)->z;
        angle = Math_Atan2(dx - x, dz - z) & 255;
    }
    if ((u32)(angle - 16) > 176)
        angle = 16;
    else if (angle > 112)
        angle = 112;
    return angle;
}

static inline void Vanargand_OffsetTarget(struct ContactPosition *v, u32 variant)
{
    if (variant == 0)
        v->x += 128;
    else
        v->z += 128;
}

static inline void Vanargand_StepSequence(struct PairedSceneHalf *part)
{
    PlayerCue_StepSequence(&part->sprite, &part->graphics.sequence);
}

static inline u32 Vanargand_GetModeShifted(u32 k_, struct PairedSceneHalf *p)
{
    return p->mode << k_;
}

static inline void Vanargand_DifferenceFromPartBase(struct ContactPosition *dst,
                                                    struct ContactPosition *a,
                                                    struct PairedSceneHalf *p)
{
    struct ContactPosition *b = p->origin;
    dst->x = a->x - b->x;
    dst->y = a->y - b->y;
    dst->z = a->z - b->z;
}

static inline void Vanargand_MakeRadialVector(struct ContactPosition *out, s32 angle, s32 scale)
{
    out->x = Vanargand_TruncateScale(gUnk_086149C4[(angle + 64) & 255] * scale);
    out->y = 0;
    out->z = Vanargand_TruncateScale(gUnk_086149C4[angle & 255] * scale);
}

static inline void Vanargand_AddVectorReversedValue(struct ContactPosition *dst,
                                                    const struct ContactPosition *a,
                                                    struct ContactPosition *b)
{
    dst->x = b->x + a->x;
    dst->y = b->y + a->y;
    dst->z = b->z + a->z;
}

static inline u32 Vanargand_StepSequenceEnded(struct PairedSceneHalf *part)
{
    return PlayerCue_StepSequenceEnded(&part->sprite, &part->graphics.sequence);
}

static inline void Vanargand_SetVector(struct ContactPosition *v, s32 x, s32 y, s32 z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}

static inline void Vanargand_AddPosition(struct ContactPosition *v, struct ContactPosition *b,
                                         struct ContactPosition *a)
{
    v->x = b->x + a->x;
    v->y = b->y + a->y;
    v->z = b->z + a->z;
}

static inline void Vanargand_FinishImpact(struct PairedSceneHalf *p, u8 *enter)
{
    u32 one;
    Vanargand_SetState(p, 3);
    one = 1;
    *enter = one;
    p->finished = one;
}

static inline void Vanargand_PolarVector(struct ContactPosition *v, s32 angle, s32 scale)
{
    v->x = Vanargand_TruncateScale(gUnk_086149C4[(angle + 64) & 255] * scale);
    v->y = 0;
    {
        s32 sample = gUnk_086149C4[angle & 255];

        v->z = Vanargand_TruncateScale(sample * scale);
    }
}

static inline void Vanargand_AddPlanar(struct ContactPosition *v, struct ContactPosition *origin)
{
    v->x = v->x + origin->x;
    v->z = v->z + origin->z;
}

static inline void Vanargand_ChangeAction(struct PairedSceneHalf *p, u8 action)
{
    u32 zero = 0;
    if ((u8)(p->requestedState - 25) <= 1 && !p->finished)
        return;
    p->requestedState = action;
    p->state = action;
    p->entered = 1;
    p->finished = zero;
    p->stateTimer = zero;
    p->actionCounters[0] = zero;
    p->actionCounters[1] = zero;
}

static inline s32 Vanargand_TerrainHeight(struct ContactPosition *v)
{
    const struct ContactPosition *position = (const struct ContactPosition *)v;
    return SceneActorTerrain_GetHeight(SceneActorTerrain_GetCellIndex(position), position);
}

static inline void Vanargand_SetY(struct ContactPosition *v, s32 y)
{
    v->y = y;
}

static inline void Vanargand_AddOffset(struct ContactPosition *out, struct ContactPosition *a,
                                       struct ContactPosition *b)
{
    out->x = (u16)b->x + (u16)a->x;
    out->y = (u16)b->y + (u16)a->y;
    out->z = (u16)b->z + (u16)a->z;
}

static inline void Vanargand_CopyRaised(struct ContactPosition *out, struct ContactPosition *origin)
{
    s32 y;
    out->x = origin->x;
    y = (u16)origin->y;
    y += 256;
    out->y = y;
    out->z = origin->z;
}

static inline void Vanargand_PolarVectorShort(struct ContactPosition *v, s32 angle, s16 scale)
{
    v->x = Vanargand_TruncateScale(gUnk_086149C4[(angle + 64) & 255] * scale);
    v->y = 0;
    {
        s32 sample = gUnk_086149C4[angle & 255];
        v->z = Vanargand_TruncateScale(sample * scale);
    }
}

static inline s32 Vanargand_Divide32(s32 value)
{
    return value >= 0 ? value >> 5 : -((-value) >> 5);
}

static inline void Vanargand_Project(struct ContactPosition *out, struct ContactPosition *position)
{
    s32 x = position->x, z = position->z;
    s32 sum, height;
    out->x = Vanargand_Divide32((x - z) * 3);
    sum = Vanargand_Divide32((x + z) * 3);
    height = Vanargand_Divide32(position->y * 3);
    out->y = sum - height;
    out->z = sum + height;
}

static inline u32 Vanargand_MaskedFlags(u32 *flags, u32 mask)
{
    return *flags & mask;
}

static inline void Vanargand_RequestAction(struct PairedSceneHalf *p, u32 action)
{
    u32 zero = 0;
    if (Vanargand_MaskedFlags(&p->actionFlags, 0x100))
        return;
    if ((u8)(p->requestedState - 25) <= 1 && !p->finished)
        return;
    p->requestedState = action;
    p->state = action;
    p->entered = 1;
    p->finished = zero;
    p->stateTimer = zero;
    p->actionCounters[0] = zero;
    p->actionCounters[1] = zero;
}

#endif
