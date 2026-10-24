#include "global.h"
// confidence=low: BossSlot2 late per-frame update: collider/shadow/camera flags and shake counters.
#define H(o) (*(s16 *)(q + (o)))
#define U(o) (*(u16 *)(q + (o)))
#define B(o) (*(u8 *)(q + (o)))
#define W(o) (*(u32 *)(q + (o)))
struct A {
    u8 pad0[0x3dc];
    u8 shadow;
    u8 pad3dd[0x4ee - 0x3dd];
    u16 t4ee;
};
struct Collider {
    u8 pad[6];
    s16 flags;
};
struct Sprite {
    u8 pad[9];
    u8 direction;
};
struct Anim {
    u8 pad[8];
    u32 flags;
};
struct Camera {
    u8 pad[0x20];
    u32 flags;
};
static inline u32 TestFlags(u32 *f, u32 mask)
{
    return *f & mask;
}
static inline void clearCameraFlags(struct Camera *a, u32 mask)
{
    a->flags &= ~mask;
}
static inline void hide(struct Anim *a)
{
    a->flags |= 1;
}
static inline void fade(struct Anim *a)
{
    a->flags = (a->flags & ~1) | 0x200;
}
static inline void unfade(struct Anim *a)
{
    a->flags &= ~0x201;
}
static inline void SetFlags(u32 *f, u32 mask)
{
    *f |= mask;
}
static inline void ClearColliderFlags(struct Collider *c, s32 mask)
{
    c->flags &= ~mask;
}
void SceneActors_UpdateEffects(void *);
void SceneActors_UpdateConvergingParticles(void *);
u32 ActorExtension_ResetState(void *);
void Collider_SetPosition(void *, void *, u32);
void SpriteComponent_Resume(void *);
void SpriteComponent_Suspend(void *);
void ProximityVolume_DispatchUpdate(void *);
s32 ActorPosition_SaveSelectedPoint(void *);
void FrostEmitter_StartEmission(void *, void *);
void FrostEmitter_StopEmission(void *);
void ActorShadow_Disable(void *);
void ActorShadow_Enable(void *);
void Camera_RequestShake(s32);
s32 ActorProperty_GetConstant34(void *);
void Camera_SetOffset(s32, void *);
void CameraTracking_Update(void *);
void SceneActors_EmitRandomBursts(void *);
void BossSlot2_UpdateSequencePalette(void *);

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
void BossSlot2_UpdatePresentation(u8 *p)
{
    u8 *q = p;
    struct Sprite *sprite = (struct Sprite *)(p + 0x48);
    u32 *flags;
    struct Collider *collider;
    u8 *grounded;
    SceneActors_UpdateEffects(p);
    SceneActors_UpdateConvergingParticles(p);
    if ((*((u8 *)(p + 0xab))) == 10) {
        if ((*((s16 *)(q + 0x150))) > 0)
            (*((s16 *)(q + 0x150)))--;
    } else
        *((s16 *)(q + 0x150)) = 0;
    {
        struct A *a = (struct A *)q;
        u32 v = a->t4ee;
        flags = (u32 *)(p + 0x9c);
        if (v) {
            SetFlags(flags, 4);
            a->t4ee--;
        }
    }
    collider = (struct Collider *)(q + 0x1cc);
    if (TestFlags(flags, 4)) {
        collider->flags |= 4;
        ActorExtension_ResetState(q);
    } else {
        ClearColliderFlags(collider, 6);
        Collider_SetPosition(collider, q + 0x2ec, 0);
        {
            u32 direction = sprite->direction;
            *((u8 *)(q + 0x20e)) = direction;
        }
    }
    collider = (struct Collider *)(q + 0x220);
    if (TestFlags(flags, 0x400000)) {
        collider->flags |= 4;
    } else {
        ClearColliderFlags(collider, 4);
        Collider_SetPosition(collider, q + 0x2ec, 0);
        {
            u32 direction = sprite->direction;
            *((u8 *)(q + 0x262)) = direction;
        }
    }
    if (TestFlags(flags, 0x100000))
        SpriteComponent_Resume(q + 0x32c);
    else
        SpriteComponent_Suspend(q + 0x32c);
    ProximityVolume_DispatchUpdate(q + 0x428);
    *((u8 *)(q + 0x477)) = ActorPosition_SaveSelectedPoint(q);
    if (TestFlags(flags, 0x200000))
        FrostEmitter_StartEmission(*((void **)(q + 0x41c)), p + 0x54);
    else
        FrostEmitter_StopEmission(*((void **)(q + 0x41c)));
    if (TestFlags(flags, 0x10)) {
        hide((struct Anim *)(q + 0x2cc));
        ActorShadow_Disable(q + 0x3dc);
        clearCameraFlags((struct Camera *)(q + 0xc30), 1);
        ClearColliderFlags((struct Collider *)(q + 0x1cc), 8);
        grounded = p + 0xb3;
    } else if (TestFlags(flags, 0x40000)) {
        fade((struct Anim *)(q + 0x2cc));
        ActorShadow_Disable(q + 0x3dc);
        clearCameraFlags((struct Camera *)(q + 0xc30), 1);
        ClearColliderFlags((struct Collider *)(q + 0x1cc), 8);
        grounded = p + 0xb3;
    } else {
        unfade((struct Anim *)(q + 0x2cc));
        {
            u8 *q2 = q;
            u32 val = *((u8 *)(p + 0xb3));
            grounded = p + 0xb3;
            {
                u8 k1 = 0x3dc;
                ActorShadow_Enable((val) ? (q2 + k1) : (q + 0x3dc));
            }
        }
        clearCameraFlags((struct Camera *)(q + 0xc30), 1);
        if (TestFlags(flags, 0x20))
            ((struct Collider *)(q + 0x1cc))->flags |= 8;
        else
            ClearColliderFlags((struct Collider *)(q + 0x1cc), 8);
    }
    if (*((u8 *)(q + 0x51f))) {
        u32 n = *((u8 *)(q + 0x51f));
        Camera_RequestShake(((s32)(n * n)) >> 2);
        (*((u8 *)(q + 0x51f)))--;
    }
    if (*((u8 *)(q + 0x511))) {
        (*((u8 *)(q + 0x511)))--;
        SetFlags(flags, 0x20000);
    }
    if ((*((s16 *)(q + 0x4ec))) > 0) {
        (*((s16 *)(q + 0x4ec)))--;
        if ((*((s16 *)(q + 0x4ec))) <= 0) {
            s32 k = ActorProperty_GetConstant34(q);
            if (k == 0)
                *((u32 *)(q + 0x314)) = (u32)(q + 0x800);
            else if (k == 1)
                *((u32 *)(q + 0x314)) = (u32)(q + 0xa00);
            else if (k == 2)
                *((u32 *)(q + 0x314)) = (u32)(q + 0xc00);
            else
                *((u32 *)(q + 0x314)) = (u32)(q + 0x600);
            *((s16 *)(q + 0x4ec)) = 0;
        }
    } else if (*((u8 *)(q + 0x506)))
        BossSlot2_UpdateSequencePalette(q);
    if (!(*grounded)) {
        if ((*((u8 *)(p + 0xab))) == 0xb)
            Camera_SetOffset(0xf, q + 0x2ec);
        else
            CameraTracking_Update(q + 0xc30);
    } else
        Camera_SetOffset(0x1e, q + 0x2ec);
    if (((*grounded) && (*((u8 *)(q + 0x51e)))) && (!((*((u32 *)(p + 0xe4))) & 0xf)))
        SceneActors_EmitRandomBursts(q);
    *((u32 *)(p + 0xbc)) = 0;
    *((u32 *)(p + 0xc0)) = 0;
    (*((u32 *)(p + 0xe4)))++;
}

#else
INCLUDE_ASM("asm/nonmatching", BossSlot2_UpdatePresentation);
#endif
