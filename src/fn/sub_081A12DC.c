#include "global.h"
// MATCHING_CONCESSION: register-pinning
#define Hresvelgr_UpdateRisingProjectile sub_081A12DC
// confidence=low: launches a moving projectile and approaches the elevated rest position.
struct Vec {
    s16 x, y, z, pad;
} __attribute__((aligned(4)));
struct Action {
    u8 state, pad[3];
    u32 frame;
};
struct Hresvelgr {
    u8 pad0[0x51];
    u8 heading;
    u8 pad52[2];
    struct Vec position;
    u8 pad5C[0x40];
    u32 flags;
    u8 padA0[8];
    u8 direction4, direction8;
    u8 padAA[5];
    u8 finished, padB0, pending, pulse;
    u8 padB3[0x19];
    u8 targetHeading;
    u8 padCD[0xa7];
    u8 animationData[0x20], animation[0x20];
    struct Vec renderPosition;
    u8 pad1BC[0x134];
    struct Action action;
    u8 pad2F8[0x24];
    s16 motion[4];
    u8 pad324[7];
    u8 side;
    u8 pad32C[0x70];
    struct Vec target;
};
extern const s16 gUnk_086149C4[];
void Hresvelgr_SetAnimation(void *, void *, void *, u32, u32, u32);
void Anim_SetFrameDurationScale(void *, u32);
void *Hresvelgr_CreateMovingProjectile(void *, void *, s32);
void Sound_PlaySong(u32);
void ActionState_Reset(void *);
void ActionState_SetState(void *, u32);
void ActionState_AdvanceState(void *);
s32 Math_StepTowardTarget(s32, s32, s32);
static inline u8 *SetHeading(struct Hresvelgr *p, s32 heading)
{
    s32 shifted;
    u32 value;
    p->heading = heading;
    shifted = (p->heading + 96) >> 6;
    value = 3;
    value &= shifted;
    p->direction4 = value;
    shifted = (p->heading + 112) >> 5;
    value = 7;
    value &= shifted;
    p->direction8 = value;
    return &p->direction4;
}
static inline s32 SignedShift(s32 value, u32 shift)
{
    if (value >= 0)
        return value >> shift;
    return -((-value) >> shift);
}
static inline void PolarVector(struct Vec *out, u32 angle, s32 magnitude)
{
    s32 value;
    angle &= 255;
    value = gUnk_086149C4[(angle + 64) & 255] * magnitude;
    out->x = SignedShift(value, 12);
    out->y = 0;
    value = gUnk_086149C4[angle] * magnitude;
    out->z = SignedShift(value, 12);
}
static inline void SetVector(struct Vec *p, s32 x, s32 y, s32 z)
{
    p->x = x;
    p->y = y;
    p->z = z;
}
static inline void CopyVector(struct Vec *out, struct Vec *source)
{
    *out = *source;
}
void Hresvelgr_UpdateRisingProjectile(struct Hresvelgr *actor)
{
    struct Hresvelgr *saved = actor;
    struct Action *action = &actor->action;
    register u8 *direction asm("r5");
    if (actor->pending) {
        struct Vec position, delta;
        actor->pending = 0;
        direction = SetHeading(actor, actor->targetHeading);
        Hresvelgr_SetAnimation(actor, actor->animation, actor->animationData, 25, 2, 4);
        Anim_SetFrameDurationScale(actor->animation, 112);
        CopyVector(&position, &actor->renderPosition);
        ((s16 *)&position)[1] += 512;
        PolarVector(&delta, ((actor->side << 6) + 128) & 255, 4);
        delta.y = 48;
        Hresvelgr_CreateMovingProjectile(&position, &delta, 1 - (*direction >> 1));
        Sound_PlaySong(0x46e);
        ActionState_Reset(action);
        ActionState_SetState(action, 0);
    }
    switch (action->state) {
        case 0:
            if (saved->pulse)
                ActionState_AdvanceState(action);
            break;
        case 1:
            if (!action->frame) {
                SetVector(&actor->target, saved->position.x, 832, saved->position.z);
                Hresvelgr_SetAnimation(actor, actor->animation, actor->animationData, 0, 1, 4);
                action->frame++;
            }
            if (saved->position.y == actor->target.y)
                saved->finished = 1;
            else
                saved->position.y = Math_StepTowardTarget(saved->position.y, actor->target.y, 4);
            if (saved->pulse)
                Sound_PlaySong(0x3d9);
            {
                u32 mask = 0x8000000;
                saved->flags |= mask;
            }
            break;
    }
    actor->motion[0] = 0;
    actor->motion[1] = 0;
    actor->motion[2] = 0;
    actor->motion[3] = 0;
    {
        u32 mask = 12;
        saved->flags |= mask;
    }
}
