#include "global.h"
// MATCHING_CONCESSION: register-pinning
#ifdef NONMATCHING
// fuzzy: 81.7%
// Heading masks, actor aliases and state-counter register allocation still differ.
#define Hresvelgr_FireProjectileAndDescend sub_081A12DC
struct ActionState {
    u8 state;
    u8 pad[3];
    u32 counter;
};
struct Vec {
    s16 x, y, z, pad;
};
extern const s16 gUnk_086149C4[];
void Hresvelgr_SetAnimation(void *, void *, void *, u32, u32, u32);
void Anim_SetFrameDurationScale(void *, u32);
void Hresvelgr_CreateMovingProjectile(struct Vec *, struct Vec *, s32);
void Sound_PlaySong(u32);
void ActionState_Reset(struct ActionState *);
void ActionState_SetState(struct ActionState *, u32);
void ActionState_AdvanceState(struct ActionState *);
s32 Math_StepTowardTarget(s32, s32, s32);
#define B(o) (*(u8 *)(p + (o)))
#define H(o) (*(u16 *)(p + (o)))
#define S(o) (*(s16 *)(p + (o)))
#define W(o) (*(u32 *)(p + (o)))
static inline s32 shift(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -(-x >> 12);
}
struct FireActor {
    u8 pad0[0x51];
    u8 heading;
    u8 pad52[2];
    s16 x, y, z;
    u8 pad5a[0x42];
    u32 flags;
    u8 padA0[8];
    u8 direction4, direction8;
    u8 padAA[5];
    u8 finished, padB0, prepared, animationEvent;
    u8 padB3[0x19];
    u8 targetAngle;
    u8 padCD[0xe7];
    struct Vec displayPosition;
    u8 pad1bc[0x134];
    struct ActionState action;
    u8 pad2f8[0x24];
    u16 motion[4];
    u8 pad324[7];
    u8 mirrored;
    u8 pad32c[0x70];
    struct Vec destination;
};
static inline void SetHeading(struct FireActor *p, s32 angle)
{
    s32 shifted;
    u16 value;
    p->heading = angle;
    shifted = p->heading + 0x60;
    shifted >>= 6;
    value = 3;
    value &= shifted;
    p->direction4 = value;
    shifted = p->heading + 0x70;
    shifted >>= 5;
    value = 7;
    value &= shifted;
    p->direction8 = value;
}
static inline void CopyPosition(struct Vec *out, struct Vec *in)
{
    *out = *in;
}
static inline void Polar(struct Vec *v, u32 angle, s32 scale)
{
    v->x = shift(gUnk_086149C4[(angle + 64) & 255] * scale);
    v->y = 0;
    v->z = shift(gUnk_086149C4[angle] * scale);
}
static inline void Raise(struct Vec *v)
{
    v->y += 0x200;
}
void Hresvelgr_FireProjectileAndDescend(struct FireActor *p)
{
    struct FireActor *actor = p;
    register struct ActionState *state asm("r8") = &p->action;
    if (p->prepared) {
        struct Vec delta, pos;
        s32 angle;
        p->prepared = 0;
        SetHeading(p, p->targetAngle);
        Hresvelgr_SetAnimation(p, (u8 *)p + 0x194, (u8 *)p + 0x174, 25, 2, 4);
        Anim_SetFrameDurationScale((u8 *)p + 0x194, 0x70);
        CopyPosition(&pos, &p->displayPosition);
        Raise(&pos);
        angle = ((p->mirrored << 6) + 128) & 255;
        Polar(&delta, angle, 4);
        delta.y = 0x30;
        Hresvelgr_CreateMovingProjectile(&pos, &delta, 1 - (p->direction4 >> 1));
        Sound_PlaySong(0x46e);
        ActionState_Reset(state);
        ActionState_SetState(state, 0);
    }
    switch (state->state) {
        case 0:
            if (p->animationEvent)
                ActionState_AdvanceState(state);
            break;
        case 1:
            if (state->counter == 0) {
                s32 x = p->x, z = p->z;
                struct Vec *v = &p->destination;
                v->x = x;
                v->y = 0x340;
                v->z = z;
                Hresvelgr_SetAnimation(p, (u8 *)p + 0x194, (u8 *)p + 0x174, 0, 1, 4);
                state->counter++;
            }
            if (p->y == p->destination.y)
                p->finished = 1;
            else
                p->y = Math_StepTowardTarget(p->y, p->destination.y, 4);
            if (p->animationEvent)
                Sound_PlaySong(0x3d9);
            p->flags |= 0x8000000;
            break;
    }
    p->motion[0] = 0;
    p->motion[1] = 0;
    p->motion[2] = 0;
    p->motion[3] = 0;
    p->flags |= 12;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081A12DC);
#endif
