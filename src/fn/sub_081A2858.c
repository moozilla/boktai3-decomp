#include "global.h"
// MATCHING_CONCESSION: register-pinning

#ifdef NONMATCHING
// fuzzy: 83.3%
// Remaining differences: flag-address lifetimes, palette constant setup and allocation.
#define Hresvelgr_UpdateCollisionPresentation sub_081A2858
// confidence=low: presentation flags inferred from collider, camera and shadow consumers.
struct Vec {
    s16 x, y, z, pad;
};
struct Collider {
    u8 pad0[6];
    s16 flags : 16;
    u8 pad8[0xc];
    struct Vec extent;
    u8 pad1c[0x26];
    u8 direction;
    u8 pad43[0x11];
};
struct Animation {
    u8 pad0[8];
    u32 flags;
};
struct Hresvelgr {
    u8 pad0[0x54];
    struct Vec position;
    u8 pad5c[0x40];
    u32 flags;
    u8 padA0;
    u8 modeFlags;
    u8 padA2[6];
    u8 direction;
    u8 padA9[0x13];
    u32 transientFlags;
    u32 transientState;
    u8 padC4[0x20];
    u8 angle;
    u8 padE5[0xaf];
    struct Animation animation;
    u8 pad1A0[0xe];
    u8 displayMode;
    u8 pad1AF[5];
    struct Vec renderPosition;
    u8 pad1BC[0x20];
    void *paletteResource;
    u8 pad1E0[0x14];
    struct Collider body;
    union {
        struct Collider contacts[2];
        struct {
            u8 pad248[0x86];
            u16 palette;
        } presentation;
    } contactBlock;
    u8 pad2F0[0x20];
    u16 shake;
    s16 flash;
    u16 invincible;
    u16 heightRamp;
    u16 blink;
    u8 pad31A[4];
    u16 contactDamage;
    u8 pad320[0xb];
    u8 colliderDirection;
    u8 pad32C[0x46];
    u16 soundMode;
    u8 pad374[0x18];
    struct Vec contactExtent;
    u8 pad394[0x20];
    u8 shadow[1];
};
extern u16 gUnk_02000550;
extern u16 gUnk_0200053C;
extern u32 gUnk_02000560;
extern const s16 gUnk_086149C4[];
void SceneActors_UpdateEffects(void *);
void Collider_SetPosition(void *, void *, s32);
void Collider_Refresh(void *);
void ScaledShadow_Enable(void *);
void ScaledShadow_Disable(void *);
void Sound_PauseChannel12(s32);
void ColliderActor_UpdateCameraOffset(void *);
void Camera_SetOffset(s32, void *);
void Camera_ResetOffset(s32);

static inline u32 TestFlags(struct Hresvelgr *p, u32 mask)
{
    return p->flags & mask;
}
static inline void SetFlags(struct Hresvelgr *p, u32 mask)
{
    p->flags |= mask;
}

static inline void HideAnimation(struct Animation *a)
{
    a->flags |= 1;
}
static inline void ShowAnimation(struct Animation *a)
{
    a->flags &= ~1;
}
static inline u32 TestGlobal(u32 mask)
{
    return gUnk_02000560 & mask;
}
static inline u32 TestMode(struct Hresvelgr *p, u32 mask)
{
    return p->modeFlags & mask;
}

static inline s32 GetHeightFactor(struct Hresvelgr *p)
{
    s32 value = gUnk_086149C4[p->angle] << 7;
    if (value >= 0)
        value >>= 12;
    else
        value = -(-value >> 12);
    return value + 64;
}

void Hresvelgr_UpdateCollisionPresentation(struct Hresvelgr *p)
{
    struct Hresvelgr *actor = p;
    struct Collider *collider;
    s32 i;
    u32 *transientFlags;
    register u32 *transientState asm("r10");
    SceneActors_UpdateEffects(p);
    if (actor->invincible) {
        SetFlags(p, 4);
        actor->invincible--;
    }
    if (actor->blink) {
        actor->blink--;
        if ((actor->blink >> 2) & 1)
            SetFlags(p, 16);
        SetFlags(p, 4);
    }
    collider = actor->contactBlock.contacts;
    if (TestFlags(p, 4)) {
        actor->contactDamage = 0;
        *(u16 *)((u8 *)actor + 0x31c) = 0;
        transientFlags = &p->transientFlags;
        transientState = &p->transientState;
        i = 1;
        do {
            collider->flags |= 4;
            collider->flags &= ~8;
            collider++;
        } while (--i >= 0);
    } else {
        collider->extent = actor->contactExtent;
        transientFlags = &p->transientFlags;
        transientState = &p->transientState;
        i = 1;
        do {
            collider->flags &= ~4;
            collider->flags |= 8;
            Collider_SetPosition(collider, &actor->renderPosition, 0);
            collider->direction = actor->colliderDirection << 6;
            collider++;
        } while (--i >= 0);
    }
    collider = &actor->body;
    if (!(TestFlags(p, 8))) {
        Collider_SetPosition(collider, &actor->renderPosition, 0);
        Collider_Refresh(collider);
    }
    if (TestFlags(p, 0x1010000))
        actor->displayMode = 3;
    else if (TestFlags(p, 0x8000000))
        actor->displayMode = 1;
    else
        actor->displayMode = 2;
    if (TestFlags(p, 16)) {
        HideAnimation(&actor->animation);
        SetFlags(p, 0x10000);
    } else {
        ShowAnimation(&actor->animation);
    }
    if (TestFlags(p, 0x10000))
        ScaledShadow_Disable(actor->shadow);
    else
        ScaledShadow_Enable(actor->shadow);
    if (TestFlags(p, 0x2000000)) {
        gUnk_02000550 = actor->soundMode;
        gUnk_0200053C = p->direction <= 1 ? 0 : 64;
    } else {
        gUnk_02000550 = 0;
        Sound_PauseChannel12(15);
    }
    if (actor->flash > 0) {
        actor->flash--;
        if (!actor->flash) {
            actor->contactBlock.presentation.palette = 0x2f0;
            actor->paletteResource = (u8 *)actor + 0x424;
        }
    }
    if ((TestFlags(p, 0x200000)) && !TestGlobal(4)) {
        if (TestMode(p, 4))
            goto fixedCamera;
        ColliderActor_UpdateCameraOffset(actor);
    } else {
        if (TestFlags(p, 2)) {
        fixedCamera:
            Camera_SetOffset(8, &p->position);
        } else {
            *((u8 *)actor + 0x382) = 0;
            Camera_ResetOffset(8);
        }
    }
    if (TestFlags(p, 0x400000)) {
        s32 factor = GetHeightFactor(p);
        actor->renderPosition.y += (actor->heightRamp * factor) / 16;
        if (actor->heightRamp <= 15)
            actor->heightRamp++;
    } else if (actor->heightRamp) {
        s32 factor = GetHeightFactor(p);
        actor->renderPosition.y += (actor->heightRamp * factor) / 16;
        actor->heightRamp--;
    }
    *transientFlags = 0;
    *transientState = 0;
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081A2858);
#endif
