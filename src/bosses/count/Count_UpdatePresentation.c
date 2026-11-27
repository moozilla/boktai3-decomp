#include "bosses/count/count.h"
#include "engine/video/render.h"
#include "npc/story_character/state_actor.h"
#include "scene/collision/contact_collider.h"
#include "scene/camera/camera_tracking.h"

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
void SceneActors_SpawnAmbientParticlePair(void *);
void SceneActors_StopBackgroundFlash(void *);

static inline u8 CountPaletteBlend_ReadAmount(struct CountPaletteBlend *blend)
{
    return blend->amount;
}

void Count_UpdatePresentation(void *arg0)
{
    struct Count *base = arg0;
    struct Count *p = arg0;
    struct SceneActorBody *sprite = &base->body;
    struct ContactCollider *collider;
    s32 cost;

    SceneActorState_SetFlags((struct StateActor *)base, 0x20);
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
    if (p->invulnerabilityTimer != 0) {
        SceneActorState_SetFlags((struct StateActor *)base, 4);
        p->invulnerabilityTimer--;
    }
    collider = &p->overlayCollider;
    if (SceneActorState_TestFlags((struct StateActor *)base, 4)) {
        ContactCollider_SetFlags(collider, 4);
    } else {
        ContactCollider_ClearFlags(collider, 4);
        Collider_SetPosition(collider, &p->animation.position, 0);
        collider->direction = sprite->direction;
    }
    collider = &p->bodyCollider;
    if (!SceneActorState_TestFlags((struct StateActor *)base, 8)) {
        Collider_SetPosition(collider, &p->animation.position, 0);
        collider->direction = sprite->direction;
        SceneActorGraphics_UpdateVariant(p);
        Collider_Refresh(collider);
    }
    if (SceneActorState_TestFlags((struct StateActor *)base, 0x10)) {
        AnimSprite_Hide(&p->animation);
        ScaledShadow_Disable(p->shadow);
        CameraTracking_ClearRequests(&p->camera, 1);
        ContactCollider_ClearFlags(&p->overlayCollider, 8);
    } else {
        AnimSprite_Show(&p->animation);
        if (base->ambientMode != 0) {
            ScaledShadow_Enable(p->shadow);
            CameraTracking_ClearRequests(&p->camera, 1);
        } else {
            ScaledShadow_Enable(p->shadow);
            CameraTracking_SetRequests(&p->camera, 1);
        }
        if (SceneActorState_TestFlags((struct StateActor *)base, 0x20)) {
            ContactCollider_SetFlags(&p->overlayCollider, 8);
        } else {
            ContactCollider_ClearFlags(&p->overlayCollider, 8);
        }
    }
    if (p->shakeAmplitude) {
        p->shakeAmplitude--;
        SceneActorState_SetFlags((struct StateActor *)base, 0x20000);
    }
    if ((s16)p->paletteRestoreTimer > 0) {
        p->paletteRestoreTimer--;
        if (p->paletteRestoreTimer == 0) {
            s32 res = SceneActorGraphics_GetResourceId(p);
            if (res == 0) {
                p->animation.palette = p->paletteNormal;
            } else if (res == 1) {
                p->animation.palette = p->paletteSpecial;
            } else {
                p->animation.palette = p->paletteDefault;
            }
        }
    } else {
        int zero = 0;
        s32 def = SceneActorGraphics_GetDefaultResourceId(p);
        s32 res = p->blend.resource;
        if (p->blend.mode == 1) {
            int amount;
            void *dst;
            u8 *amountByte = (u8 *)&p->blend;
            amount = (s8)*amountByte;
            if (amount < (1 << p->blend.shift)) {
                u8 *a;
                u8 *b;
                *amountByte += 1;
                a = gPaletteBankData + (def << 5);
                b = gPaletteBankData + (res << 5);
                dst = p->paletteDefault;
                Palette_InterpolateBanks(
                    dst, a, b, (s8)CountPaletteBlend_ReadAmount((struct CountPaletteBlend *)amountByte),
                    p->blend.shift);
                p->animation.palette = dst;
            }
        } else if (p->blend.mode == 2) {
            void *dst;
            u8 *amountByte = (u8 *)&p->blend;
            if ((s8)*amountByte > 0) {
                u8 *a;
                u8 *b;
                *amountByte -= 1;
                a = gPaletteBankData + (def << 5);
                b = gPaletteBankData + (res << 5);
                dst = p->paletteDefault;
                Palette_InterpolateBanks(
                    dst, a, b, (s8)CountPaletteBlend_ReadAmount((struct CountPaletteBlend *)amountByte),
                    p->blend.shift);
                p->animation.palette = dst;
            } else {
                SceneActorGraphics_SelectResource(p);
            }
        } else if (p->blend.mode == 3) {
            void *dst;
            u8 *amountByte;
            u8 *a = gPaletteBankData + (def << 5);
            u8 *b = gPaletteBankData + (res << 5);
            dst = p->paletteDefault;
            amountByte = (u8 *)&p->blend;
            Palette_InterpolateBanks(dst, a, b, (s8)*amountByte, p->blend.shift);
            p->animation.palette = dst;
            if (p->blend.descending == 0) {
                *amountByte += 1;
                if ((s8)*amountByte > 0x1f) {
                    *amountByte = 0x20;
                    p->blend.descending = 1;
                }
            } else {
                *amountByte -= 1;
                if ((s8)*amountByte <= 0) {
                    *amountByte = zero;
                    p->blend.descending = zero;
                }
            }
        }
    }
    if (base->ambientMode == 0)
        CameraTracking_Update(&p->camera);
    if (base->ambientMode != 0 && p->ambientParticles && (base->effectTimer & 0xf) == 0)
        SceneActors_SpawnAmbientParticlePair(p);
    if (p->backgroundFlashTimer > 0) {
        p->backgroundFlashTimer--;
        if (p->backgroundFlashTimer == 0)
            SceneActors_StopBackgroundFlash(p);
    }
    base->contactAttack = 0;
    base->contactFlags = 0;
    base->effectTimer++;
}
