#include "global.h"
#include "motion_track.h"
#include "contact_swarm_sprite.h"

// confidence=low: initializes a half's sprite, origin, collision, target registration and shadow.
void AnimData_Load(void *, u32);
void Sprite_Init(void *, void *, u32);
void ActorStatus_InitializeCollider(void *, s32, s32, s32);
void ActorStatus_InitializeBodyCollider(void *);
void ActorStatus_BeginTimedState(void);
void TargetRegistry_RegisterSceneActor(void *, void *, void (*)(void));
void ScaledShadow_Initialize(void *, void *, s32, s32, s32, s32, s32, s32, s32, s32, s32);
void ScaledShadow_SetGraphic(void *, u32, s32);
void ScaledShadow_Enable(void *);
static inline void FillOffset(void *destination, u32 value)
{
    CpuSet(&value, destination, 0x05000002);
}

struct PairedSceneHalf {
    struct ContactSwarmSprite sprite;
    u8 graphics[0x2C];
    struct MotionTrack motion;
    void *origin;
    u8 unknown94[8];
    struct ContactPosition offset;
    u8 unknownA4[8];
    u16 heading, scale, fieldB0, fieldB2;
    u8 unknownB4[0x54];
    u8 damageCollider[0x54];
    s32 mode;
    u8 unknown160[0xC];
    u8 state, requestedState, entered, unknown16F, finished, unknown171;
    u16 active, health, maxHealth;
    u32 stateTimer, hitFlags;
    u16 damage, specialDamage, flashTimer;
    u8 field186, field187;
    u16 field188;
    u8 unknown18A[0x12];
    u32 shake;
    u8 shadow[0x6C];
    u8 target[0x10];
};
void PairedSceneActor_InitializeHalf(struct PairedSceneHalf *half, void *origin, s32 mode)
{
    struct PairedSceneHalf *part = half;
    void *data;
    void *shadow;
    u16 zero;
    u16 clearedState;
    u32 scale;
    u32 one;
    u32 initialFrame;
    part->mode = mode;
    data = part->graphics;
    AnimData_Load(data, 0x89CA);
    Sprite_Init(half, data, 0);
    initialFrame = 0;
    part->sprite.frame = initialFrame;
    part->sprite.priority = 2;
    if (mode == 1)
        part->sprite.flags |= 4;
    part->sprite.flags |= 1;
    FillOffset(&part->offset, initialFrame);
    part->origin = origin;
    if (mode == 0)
        part->heading = 64;
    else
        part->heading = initialFrame;
    {
        u16 *size = &half->scale;
        clearedState = 0;
        zero = initialFrame;
        scale = 1024;
        *size = scale;
    }
    {
        u16 *fieldB0 = &half->fieldB0;
        *fieldB0 = 3;
    }
    {
        u16 *active = &half->active;
        one = 1;
        *active = one;
    }
    ActorStatus_InitializeCollider(half, 30, 128, 60);
    ActorStatus_InitializeBodyCollider(half);
    {
        u16 *field188 = &half->field188;
        *field188 = zero;
    }
    half->state = clearedState;
    half->requestedState = clearedState;
    {
        struct MotionTrack *track = &half->motion;
        track->data = (void *)(u32)zero;
        half->motion.active = clearedState;
        half->motion.mode = clearedState;
        track->cursor = zero;
    }
    TargetRegistry_RegisterSceneActor(half->target, half->damageCollider,
                                      ActorStatus_BeginTimedState);
    shadow = half->shadow;
    ScaledShadow_Initialize(shadow, &half->sprite.position, 128, 128, 128, 128, scale, one, 6, zero,
                            zero);
    ScaledShadow_SetGraphic(shadow, 0x1C1B, 7);
    ScaledShadow_Enable(shadow);
    {
        u16 *damage = &half->damage;
        *damage = zero;
    }
    half->hitFlags = zero;
    {
        u16 *specialDamage = &half->specialDamage;
        *specialDamage = zero;
    }
    half->field186 = clearedState;
    {
        u16 *flashTimer = &half->flashTimer;
        *flashTimer = zero;
    }
    half->field187 = clearedState;
    half->shake = zero;
    half->fieldB2 = one;
    half->maxHealth = 300;
    half->health = zero;
}
