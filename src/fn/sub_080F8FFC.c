#include "rotation_phase.h"
#define Axe_UpdateTargetLunge sub_080F8FFC
#ifdef NONMATCHING
// fuzzy: 85.9%
// Remaining: player-vector argument order, flags pointer lifetime, rotation scratch and phase-tail scheduling.
// confidence=low: launches toward the player and brakes sprite rotation on arrival.
#define F(p, o, t) (*(t *)((u8 *)(p) + (o)))
extern u8 *gUnk_02000580;
s32 Div(s32, s32);
s32 Mod(s32, s32);
void Sound_PlaySong(u32);
void Axe_EmitMotionTrail(void *);
void sub_080FC094(void *, s32);
void Axe_SetAuxiliarySpriteRenderState(void *, void *, u32);
void SceneActorControl_FinishRotation(void);
static inline void Delta(struct ContactPosition *v, struct ContactPosition *a,
                         struct ContactPosition *b)
{
    v->x = a->x - b->x;
    v->y = a->y - b->y;
    v->z = a->z - b->z;
}
static inline s32 Shift(s32 v)
{
    return v >= 0 ? v >> 6 : -(-v >> 6);
}
static inline void BeginControl(struct SceneActorRegistryResetView *a, void *callback, u8 phase)
{
    u32 zero = 0;
    a->field2B2 = 1;
    a->field2B0 = zero;
    a->field2B5 = phase;
    a->target = callback;
    a->field2C8 = zero;
    a->flags114 &= ~1;
    a->word104 = zero;
}
void Axe_UpdateTargetLunge(struct SceneActorRegistryResetView *a, s32 frame)
{
    u8 *state = a->context;
    struct RotatingDrawable *sprite = a->drawable;
    u8 entered = SceneActorControl_TakeFlag(&a->field2B2);
    s32 angle;
    u32 *flags = &a->flags2D0;
    if (entered) {
        struct ContactPosition delta;
        sprite->flags |= 2;
        state[0xEAE] = 2;
        Delta(&delta, (void *)(gUnk_02000580 + 0x30), &a->position);
        if (a->health <= F(state, 0xE48, s32)) {
            F(a, 0x2EC, s16) = Div(delta.x, 44);
            F(a, 0x2EE, s16) = 44;
            F(a, 0x2F0, s16) = Div(delta.z, 44);
        } else {
            F(a, 0x2EC, s16) = Shift(delta.x);
            F(a, 0x2EE, s16) = 64;
            F(a, 0x2F0, s16) = Shift(delta.z);
        }
        state[0xEB3] = 0;
        *flags &= ~0x100000;
        Axe_SetAuxiliarySpriteRenderState(a, state + 0x8A8, 0x13D);
        Sound_PlaySong(0x1E5);
    }
    a->position.y += F(a, 0x2EA, u16);
    if (NormalizeAngle(32 - state[0xED3]) < 0)
        sprite->angle += state[0xEAE];
    else
        sprite->angle -= state[0xEAE];
    angle = NormalizeAngle(sprite->angle);
    Axe_EmitMotionTrail(a);
    if (frame <= 27) {
        if (!Mod(frame, 10))
            state[0xEAE] += 4;
    } else {
        s32 magnitude = angle;
        if (magnitude < 0)
            magnitude = -magnitude;
        if (magnitude <= 95)
            state[0xEAE] = 8;
        else
            state[0xEAE] = 0;
    }
    if (*flags & 0x10) {
        u32 zero = 0;
        state[0xED1] = 1;
        BeginControl(a, SceneActorControl_FinishRotation, 0x33);
        F(state, 0xE4C, u32) &= ~0x800;
        state[0xEB2] = 64;
        sub_080FC094(a, 14);
    }
}

#else
INCLUDE_ASM("asm/nonmatching", sub_080F8FFC);
#endif
