#include "global.h"
#include "engine/ram.h"
#include "bosses/hresvelgr/hresvelgr_travel.h"

// confidence=low: synchronizes collision, shadow and camera presentation.

extern u32 gUnk_02000560;
extern const s16 gUnk_086149C4[];
void SceneActors_UpdateEffects(void *);
void Collider_SetPosition(void *, void *, s32);
void Collider_Refresh(void *);
void ScaledShadow_Disable(void *);
void ScaledShadow_Enable(void *);
void Sound_PauseChannel12(s32);
void ColliderActor_UpdateCameraOffset(void *);
void Camera_SetOffset(s32, void *);
void Camera_ResetOffset(s32);
static inline s32 shift(s32 x)
{
    if (x >= 0)
        return x >> 12;
    return -(-x >> 12);
}
static inline u32 test(u32 *p, u32 mask)
{
    return *p & mask;
}
static inline void SetSpriteVisibility(struct AnimSprite *sprite, s32 hidden)
{
    if (hidden)
        sprite->flags |= 1;
    else
        sprite->flags &= ~1;
}
static inline void RestorePalette(struct HresvelgrTravelActor *p, u16 bank)
{
    p->animation.paletteBank = bank;
    p->animation.palette = p->normalPalette[0];
}
void Hresvelgr_UpdateCollisionPresentation(struct HresvelgrTravelActor *p)
{
    struct HresvelgrTravelActor *p2 = p;
    struct ContactCollider *c;
    s32 i;
    u32 *flags;
    u32 *resetB, *resetA;
    SceneActors_UpdateEffects(p);
    if (p->invulnerabilityTimer) {
        u32 mask = 4;
        p->flags |= mask;
        p->invulnerabilityTimer--;
    }
    {
        u16 invuln = p->recoveryFrames;
        flags = &p->flags;
        if (invuln) {
            p->recoveryFrames--;
            if ((p->recoveryFrames >> 2) & 1) {
                u32 mask = 16;
                *flags |= mask;
            }
            {
                u32 mask = 4;
                *flags |= mask;
            }
        }
    }
    c = p->bodyColliders;
    if (test(&*flags, 4)) {
        p->motionY = 0;
        p->motionX = 0;
        resetA = &p2->contactFlags;
        resetB = &p2->contactAttributes;
        for (i = 0; i < 2; i++) {
            c->flags |= 4;
            c->flags = (s16)c->flags & ~8;
            c++;
        }
    } else {
        c->offset = p->bodyOffset;
        resetA = &p2->contactFlags;
        resetB = &p2->contactAttributes;
        for (i = 0; i < 2; i++) {
            {
                s32 mask = ~4;
                c->flags = (c->flags & mask) | 8;
            }
            Collider_SetPosition(c, &p->animation.position, 0);
            c->direction = p->side << 6;
            c++;
        }
    }
    c = &p->attackCollider;
    if (!test(&*flags, 8)) {
        Collider_SetPosition(c, &p->animation.position, 0);
        Collider_Refresh(c);
    }
    if (test(flags, 0x1010000))
        p->animation.attribute1A = 3;
    else if (*flags & 0x8000000)
        p->animation.attribute1A = 1;
    else
        p->animation.attribute1A = 2;
    if (test(&*flags, 16)) {
        SetSpriteVisibility(&p->animation, 1);
        {
            u32 mask = 0x10000;
            *flags |= mask;
        }
    } else
        SetSpriteVisibility(&p->animation, 0);
    if (test(&*flags, 0x10000))
        ScaledShadow_Disable(p->scaledShadow);
    else
        ScaledShadow_Enable(p->scaledShadow);
    if (test(&*flags, 0x2000000)) {
        gPlayerBackgroundState = p->backgroundState;
        gPlayerBackgroundMode = p2->facing4 <= 1 ? 0 : 64;
    } else {
        gPlayerBackgroundState = 0;
        Sound_PauseChannel12(15);
    }
    if (p->paletteRestoreTimer > 0) {
        p->paletteRestoreTimer--;
        if (!p->paletteRestoreTimer) {
            RestorePalette(p, 0x2F0);
        }
    }
    if (test(flags, 0x200000)) {
        u32 cameraMask = 4;
        if (gUnk_02000560 & cameraMask)
            goto defaultCamera;
        if (!(p2->behaviorFlags & cameraMask)) {
            ColliderActor_UpdateCameraOffset(p);
            goto cameraDone;
        }
        Camera_SetOffset(8, &p2->position);
    } else {
    defaultCamera:
        if (test(flags, 2)) {
            Camera_SetOffset(8, &p2->position);
        } else {
            p->cameraState = 0;
            Camera_ResetOffset(8);
        }
    }
cameraDone:
    if (test(&*flags, 0x400000)) {
        s32 k = shift(gUnk_086149C4[p2->swayAngle] << 7) + 64;
        p->animation.position.y += (p->verticalSway * k) / 16;
        if (p->verticalSway <= 15)
            p->verticalSway++;
    } else if (p->verticalSway) {
        s32 k = shift(gUnk_086149C4[p2->swayAngle] << 7) + 64;
        p->animation.position.y += (p->verticalSway * k) / 16;
        p->verticalSway--;
    }
    *resetA = 0;
    *resetB = 0;
}
