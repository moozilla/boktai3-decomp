#include "global.h"
#include "scene/camera/camera_tracking.h"
#include "scene/collision/contact_collider.h"
#include "engine/word_flags.h"
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
struct Sprite {
    u8 pad[9];
    u8 direction;
};
struct Anim {
    u8 pad[8];
    u32 flags;
};
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
void SceneActors_UpdateEffects(void *);
void SceneActors_UpdateConvergingParticles(void *);
u32 ActorExtension_ResetState(void *);
void Collider_SetPosition(void *, void *, u32);
void SpriteComponent_Resume(void *);
void SpriteComponent_Suspend(void *);
void ProximityVolume_DispatchUpdate(void *);
s32 SabataPosition_SaveSelectedPoint(void *);
void SabataFrost_StartEmission(void *, void *);
void SabataFrost_StopEmission(void *);
void ActorShadow_Disable(void *);
void ActorShadow_Enable(void *);
void Camera_RequestShake(s32);
s32 ActorProperty_GetConstant34(void *);
void Camera_SetOffset(s32, void *);
void SceneActors_EmitRandomBursts(void *);
void Sabata_UpdateSequencePalette(void *);

void Sabata_UpdatePresentation(u8 *p)
{
    u8 *q = p;
    struct Sprite *sprite = (struct Sprite *)(p + 0x48);
    u32 *flags;
    struct ContactCollider *collider;
    u8 *grounded;
    SceneActors_UpdateEffects(p);
    SceneActors_UpdateConvergingParticles(p);
    if ((*(u8 *)(p + 0xab)) == 10) {
        if (H(0x150) > 0)
            H(0x150)--;
    } else
        H(0x150) = 0;
    {
        struct A *a = (struct A *)q;
        u32 v = a->t4ee;
        flags = (u32 *)(p + 0x9c);
        if (v) {
            WordFlags_Set(4, flags);
            a->t4ee--;
        }
    }
    collider = (struct ContactCollider *)(q + 0x1cc);
    if (WordFlags_Test(flags, 4)) {
        collider->flags |= 4;
        ActorExtension_ResetState(q);
    } else {
        ContactCollider_ClearFlags(collider, 6);
        Collider_SetPosition(collider, q + 0x2ec, 0);
        {
            u32 direction = sprite->direction;
            B(0x20e) = direction;
        }
    }
    collider = (struct ContactCollider *)(q + 0x220);
    if (WordFlags_Test(flags, 0x400000)) {
        collider->flags |= 4;
    } else {
        ContactCollider_ClearFlags(collider, 4);
        Collider_SetPosition(collider, q + 0x2ec, 0);
        {
            u32 direction = sprite->direction;
            B(0x262) = direction;
        }
    }
    if (WordFlags_Test(flags, 0x100000))
        SpriteComponent_Resume(q + 0x32c);
    else
        SpriteComponent_Suspend(q + 0x32c);
    ProximityVolume_DispatchUpdate(q + 0x428);
    B(0x477) = SabataPosition_SaveSelectedPoint(q);
    if (WordFlags_Test(flags, 0x200000))
        SabataFrost_StartEmission(*(void **)(q + 0x41c), p + 0x54);
    else
        SabataFrost_StopEmission(*(void **)(q + 0x41c));
    if (WordFlags_Test(flags, 0x10)) {
        hide((struct Anim *)(q + 0x2cc));
        ActorShadow_Disable(&((struct A *)q)->shadow);
        CameraTracking_ClearRequests((struct CameraTracking *)(q + 0xc30), 1);
        ContactCollider_ClearFlags((struct ContactCollider *)(q + 0x1cc), 8);
        grounded = p + 0xb3;
    } else if (WordFlags_Test(flags, 0x40000)) {
        fade((struct Anim *)(q + 0x2cc));
        ActorShadow_Disable(&((struct A *)q)->shadow);
        CameraTracking_ClearRequests((struct CameraTracking *)(q + 0xc30), 1);
        ContactCollider_ClearFlags((struct ContactCollider *)(q + 0x1cc), 8);
        grounded = p + 0xb3;
    } else {
        unfade((struct Anim *)(q + 0x2cc));
        {
            struct A *shadowActor = (struct A *)q;
            u32 val = *(u8 *)(p + 0xb3);
            grounded = p + 0xb3;
            if (val) {
                ActorShadow_Enable(&shadowActor->shadow);
                CameraTracking_ClearRequests((struct CameraTracking *)(q + 0xc30), 1);
            } else {
                ActorShadow_Enable(&shadowActor->shadow);
                CameraTracking_ClearRequests((struct CameraTracking *)(q + 0xc30), 1);
            }
        }
        if (WordFlags_Test(flags, 0x20))
            ((struct ContactCollider *)(q + 0x1cc))->flags |= 8;
        else
            ContactCollider_ClearFlags((struct ContactCollider *)(q + 0x1cc), 8);
    }
    if (B(0x51f)) {
        u32 n = B(0x51f);
        Camera_RequestShake((s32)(n * n) >> 2);
        B(0x51f)--;
    }
    if (B(0x511)) {
        B(0x511)--;
        WordFlags_Set(0x20000, flags);
    }
    if (H(0x4ec) > 0) {
        H(0x4ec)--;
        if (H(0x4ec) <= 0) {
            s32 k = ActorProperty_GetConstant34(q);
            if (k == 0)
                W(0x314) = (u32)(q + 0x800);
            else if (k == 1)
                W(0x314) = (u32)(q + 0xa00);
            else if (k == 2)
                W(0x314) = (u32)(q + 0xc00);
            else
                W(0x314) = (u32)(q + 0x600);
            H(0x4ec) = 0;
        }
    } else if (B(0x506))
        Sabata_UpdateSequencePalette(q);
    if (!*grounded) {
        if ((*(u8 *)(p + 0xab)) == 0xb)
            Camera_SetOffset(0xf, q + 0x2ec);
        else
            CameraTracking_Update(q + 0xc30);
    } else
        Camera_SetOffset(0x1e, q + 0x2ec);
    if (*grounded && B(0x51e) && !((*(u32 *)(p + 0xe4)) & 0xf))
        SceneActors_EmitRandomBursts(q);
    (*(u32 *)(p + 0xbc)) = 0;
    (*(u32 *)(p + 0xc0)) = 0;
    (*(u32 *)(p + 0xe4))++;
}
