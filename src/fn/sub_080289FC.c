#include "global.h"
#include "render.h"

// confidence=low: per-frame scene actor update: applies queued health costs, drives the overlay and body
// collider state from the action flags, runs two countdowns, picks the palette bank, steps the effect
// scale by effect mode, updates camera tracking and ambient particles, and advances the flash countdown.
s32 SceneActors_SetHealth(void *, s32);
void SceneActors_UpdateEffects(void *);
void Collider_SetPosition(void *, void *, u16);
s32 Collider_Refresh(void *);
void SceneActorGraphics_UpdateVariant(void *);
void ScaledShadow_Enable(void *);
void ScaledShadow_Disable(void *);
s32 SceneActorGraphics_GetResourceId(void *);
s32 SceneActorGraphics_GetDefaultResourceId(void *);
void SceneActorGraphics_SelectResource(void *);
void Palette_InterpolateBanks(void *, void *, void *, s32, s32);
void CameraTracking_Update(void *);
void SceneActors_SpawnAmbientParticlePair(void *);
void SceneActors_StopBackgroundFlash(void *);

struct FrameSprite {
    u8 pad0[9];
    u8 direction;
    u8 padA[0x42];
};
union FrameFlags {
    u32 value;
    u8 bytes[4];
};
struct FrameBase {
    u8 pad0[0x48];
    struct FrameSprite sprite;
    s16 health;
    u8 pad96[6];
    union FrameFlags flags;
    u8 padA0[0x13];
    u8 ambientMode;
    u8 padB4[8];
    u32 requestX, requestZ;
    u8 padC4[0x20];
    u32 ticks;
};
struct FrameCollider {
    u8 pad0[6];
    u16 flags;
    u8 pad8[0x3a];
    u8 direction;
    u8 pad43[0x11];
};
struct FrameAnimation {
    u8 pad0[8];
    u32 flags;
    u8 padC[0x14];
};
struct FrameCamera {
    u8 pad0[0x20];
    u32 flags;
};
struct FrameBlendState {
    union {
        struct {
            u32 amount : 8;
            u32 descending : 8;
            u32 mode : 8;
            u32 shift : 8;
        } fields;
        struct {
            u8 amount;
            u8 descending;
            u8 mode;
            u8 shift;
        } bytes;
    } control;
    s32 resource;
};
struct VampireFrame {
    struct FrameBase base;
    u8 padE8[0x90];
    struct FrameCollider body;
    struct FrameCollider overlay;
    u8 pad220[0x20];
    struct FrameAnimation animation;
    u8 position[8];
    u8 pad268[0x20];
    void *palette;
    u8 pad28C[0x14];
    u8 shadow[0xac];
    u16 flashTimer;
    u16 effectTimer;
    u8 pad350[0x3c];
    struct FrameBlendState blend;
    u8 pad394[5];
    u8 shakeAmplitude;
    u8 pad39A[2];
    s32 queuedHeal;
    s32 queuedDamage;
    u8 pad3A4[0x14];
    u8 ambientParticles;
    u8 pad3B9[7];
    s32 flashFrames;
    u8 paletteDefault[0x200];
    u8 paletteNormal[0x200];
    u8 paletteSpecial[0x200];
    u8 pad9C4[0x1c];
    struct FrameCamera camera;
};

static inline void Frame_AddFlags(struct FrameBase *p, u32 mask)
{
    p->flags.value |= mask;
}
static inline u32 Frame_TestFlags(struct FrameBase *p, u32 mask)
{
    return p->flags.value & mask;
}
#define Frame_SetColliderFlags(collider, mask) ((collider)->flags |= (mask))
static inline void Frame_ClearColliderFlags(struct FrameCollider *collider, u32 mask)
{
    u32 keep = ~mask;
    collider->flags = keep & collider->flags;
}

static inline void Frame_HideAnimation(struct FrameAnimation *animation)
{
    animation->flags |= 1;
}
static inline void Frame_ClearAnimationFlags(struct FrameAnimation *animation, u32 mask)
{
    u32 flags = animation->flags;
    flags &= ~mask;
    animation->flags = flags;
}
static inline void Frame_SetCameraFlags(struct FrameCamera *camera, u32 mask)
{
    camera->flags |= mask;
}
static inline void Frame_ClearCameraFlags(struct FrameCamera *camera, u32 mask)
{
    u32 keep = ~mask;
    camera->flags &= keep;
}

static inline u8 Frame_ReadBlendAmount(struct FrameBlendState *blend)
{
    return blend->control.fields.amount;
}

void sub_080289FC(void *arg0)
{
    struct FrameBase *base = arg0;
    struct VampireFrame *p = arg0;
    struct FrameSprite *sprite = &base->sprite;
    struct FrameCollider *collider;
    s32 cost;

    Frame_AddFlags(base, 0x20);
    cost = p->queuedHeal;
    if (cost != 0) {
        SceneActors_SetHealth(base, base->health + cost);
        p->queuedHeal = 0;
    }
    cost = p->queuedDamage;
    if (cost != 0) {
        SceneActors_SetHealth(base, base->health - cost);
        p->queuedDamage = 0;
    }
    SceneActors_UpdateEffects(base);
    if (p->effectTimer != 0) {
        Frame_AddFlags(base, 4);
        p->effectTimer--;
    }
    collider = &p->overlay;
    if (Frame_TestFlags(base, 4)) {
        Frame_SetColliderFlags(collider, 4);
    } else {
        Frame_ClearColliderFlags(collider, 4);
        Collider_SetPosition(collider, p->position, 0);
        collider->direction = sprite->direction;
    }
    collider = &p->body;
    if (!Frame_TestFlags(base, 8)) {
        Collider_SetPosition(collider, p->position, 0);
        collider->direction = sprite->direction;
        SceneActorGraphics_UpdateVariant(p);
        Collider_Refresh(collider);
    }
    if (Frame_TestFlags(base, 0x10)) {
        Frame_HideAnimation(&p->animation);
        ScaledShadow_Disable(p->shadow);
        Frame_ClearCameraFlags(&p->camera, 1);
        Frame_ClearColliderFlags(&p->overlay, 8);
    } else {
        Frame_ClearAnimationFlags(&p->animation, 1);
        if (base->ambientMode != 0) {
            ScaledShadow_Enable(p->shadow);
            Frame_ClearCameraFlags(&p->camera, 1);
        } else {
            ScaledShadow_Enable(p->shadow);
            Frame_SetCameraFlags(&p->camera, 1);
        }
        if (Frame_TestFlags(base, 0x20)) {
            Frame_SetColliderFlags(&p->overlay, 8);
        } else {
            Frame_ClearColliderFlags(&p->overlay, 8);
        }
    }
    if (p->shakeAmplitude) {
        p->shakeAmplitude--;
        Frame_AddFlags(base, 0x20000);
    }
    if ((s16)p->flashTimer > 0) {
        p->flashTimer--;
        if (p->flashTimer == 0) {
            s32 res = SceneActorGraphics_GetResourceId(p);
            if (res == 0) {
                p->palette = p->paletteNormal;
            } else if (res == 1) {
                p->palette = p->paletteSpecial;
            } else {
                p->palette = p->paletteDefault;
            }
        }
    } else {
        int zero = 0;
        s32 def = SceneActorGraphics_GetDefaultResourceId(p);
        s32 res = p->blend.resource;
        if (p->blend.control.fields.mode == 1) {
            int amount;
            void *dst;
            u8 *dur;
            u8 *effScale = &p->blend.control.bytes.amount;
            amount = (s8)*effScale;
            dur = &p->blend.control.bytes.shift;
            if (amount < (1 << *dur)) {
                u8 *a;
                u8 *b;
                *effScale += 1;
                a = gPaletteBankData + (def << 5);
                b = gPaletteBankData + (res << 5);
                dst = p->paletteDefault;
                Palette_InterpolateBanks(
                    dst, a, b, (s8)Frame_ReadBlendAmount((struct FrameBlendState *)effScale), *dur);
                p->palette = dst;
            }
        } else if (p->blend.control.fields.mode == 2) {
            void *dst;
            u8 *effScale = &p->blend.control.bytes.amount;
            if ((s8)*effScale > 0) {
                u8 *a;
                u8 *b;
                *effScale -= 1;
                a = gPaletteBankData + (def << 5);
                b = gPaletteBankData + (res << 5);
                dst = p->paletteDefault;
                Palette_InterpolateBanks(
                    dst, a, b, (s8)Frame_ReadBlendAmount((struct FrameBlendState *)effScale),
                    p->blend.control.fields.shift);
                p->palette = dst;
            } else {
                SceneActorGraphics_SelectResource(p);
            }
        } else if (p->blend.control.fields.mode == 3) {
            void *dst;
            u8 *effScale;
            u8 *a = gPaletteBankData + (def << 5);
            u8 *b = gPaletteBankData + (res << 5);
            dst = p->paletteDefault;
            effScale = &p->blend.control.bytes.amount;
            Palette_InterpolateBanks(dst, a, b, (s8)*effScale, p->blend.control.fields.shift);
            p->palette = dst;
            if (p->blend.control.fields.descending == 0) {
                *effScale += 1;
                if ((s8)*effScale > 0x1f) {
                    *effScale = 0x20;
                    p->blend.control.fields.descending = 1;
                }
            } else {
                *effScale -= 1;
                if ((s8)*effScale <= 0) {
                    *effScale = zero;
                    p->blend.control.fields.descending = zero;
                }
            }
        }
    }
    if (base->ambientMode == 0)
        CameraTracking_Update(&p->camera);
    if (base->ambientMode != 0 && p->ambientParticles && (base->ticks & 0xf) == 0)
        SceneActors_SpawnAmbientParticlePair(p);
    if (p->flashFrames > 0) {
        p->flashFrames--;
        if (p->flashFrames == 0)
            SceneActors_StopBackgroundFlash(p);
    }
    base->requestX = 0;
    base->requestZ = 0;
    base->ticks++;
}
