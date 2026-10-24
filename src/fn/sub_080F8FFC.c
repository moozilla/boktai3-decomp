#include "rotation_phase.h"

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
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
    {
        typeof(a->field2B2) *q1 = &a->field2B2;
        *q1 = 1;
    }
    a->field2B0 = zero;
    a->field2B5 = phase;
    a->target = callback;
    a->field2C8 = zero;
    a->flags114 &= ~1;
    a->word104 = zero;
}

static inline u32 Arg1(u32 k_, u32 *flags)
{
    return (*flags) & k_;
}

inline static void BeginControl_UpdateTargetLunge(struct SceneActorRegistryResetView *a,
                                                  void *callback, u8 phase)
{
    u32 zero = 0;
    {
        typeof(a->field2B2) *q1 = &a->field2B2;
        *q1 = 1;
    }
    a->field2B0 = zero;
    a->field2B5 = phase;
    a->target = callback;
    a->field2C8 = zero;
    {
        u8 k1 = 1;
        a->flags114 &= ~k1;
    }
    a->word104 = zero;
}

inline static u32 Arg2(u32 k_)
{
    return gUnk_02000580 + k_;
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
        Delta(&delta, (void *)Arg2(0x30), &a->position);
        if (a->health <= (*((s32 *)(((u8 *)state) + 0xE48)))) {
            *((s16 *)(((u8 *)a) + 0x2EC)) = Div(delta.x, 44);
            *((s16 *)(((u8 *)a) + 0x2EE)) = 44;
            *((s16 *)(((u8 *)a) + 0x2F0)) = Div(delta.z, 44);
        } else {
            *((s16 *)(((u8 *)a) + 0x2EC)) = Shift(delta.x);
            *((s16 *)(((u8 *)a) + 0x2EE)) = 64;
            *((s16 *)(((u8 *)a) + 0x2F0)) = Shift(delta.z);
        }
        state[0xEB3] = 0;
        *flags &= ~0x100000;
        Axe_SetAuxiliarySpriteRenderState(a, state + 0x8A8, 0x13D);
        Sound_PlaySong(0x1E5);
    }
    a->position.y += *((u16 *)(((u8 *)a) + 0x2EA));
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
    if (Arg1(0x10, flags)) {
        u32 zero = 0;
        state[0xED1] = 1;
        BeginControl_UpdateTargetLunge(a, SceneActorControl_FinishRotation, 0x33);
        {
            u16 k1 = 0x800;
            *((u32 *)(((u8 *)state) + 0xE4C)) &= ~k1;
        }
        state[0xEB2] = 64;
        sub_080FC094(a, 14);
    }
}

#else
INCLUDE_ASM("asm/nonmatching", Axe_UpdateTargetLunge);
#endif
