#include "global.h"
#include "render.h"

// confidence=low: per-frame scene actor update: applies queued health costs, drives the overlay and body
// collider state from the action flags, runs two countdowns, picks the palette bank, steps the effect
// scale by effect mode, then runs the locked-camera or ambient-particle branch and the flash countdown.
void SceneActors_SetHealth(void *, s32);
void SceneActors_UpdateEffects(void *);
void Collider_SetPosition(void *, void *, u32);
s32 Collider_Refresh(void *);
void SceneActorGraphics_UpdateVariant(void *);
void ScaledShadow_Enable(void *);
void ScaledShadow_Disable(void *);
s32 SceneActorGraphics_GetResourceId(void *);
s32 SceneActorGraphics_GetDefaultResourceId(void *);
void SceneActorGraphics_SelectResource(void *);
void Palette_InterpolateBanks(void *, void *, void *, u32, u32);
void CameraTracking_Update(void *);
void SceneActors_SpawnAmbientParticlePair(void *);
void SceneActors_StopBackgroundFlash(void *);

#ifdef NONMATCHING
// fuzzy: 37.5%
// scope shape 196 (differences once low registers are set aside)
// confidence=low: per-frame scene actor update: applies queued health costs, drives the overlay and body
// collider state from the action flags, runs two countdowns, picks the palette bank, steps the effect
// scale by effect mode, then runs the locked-camera or ambient-particle branch and the flash countdown.
void sub_080289FC(void *arg0)
{
    u8 *p = (u8 *)arg0;
    u8 *b48 = p + 0x48;
    u8 *ov = p + 0x1CC;
    u8 *body;
    u8 *dst;
    u8 *b240;
    u8 *b9E0;
    u32 *flags = (u32 *)(p + 0x9C);
    s32 cost;
    s32 res;
    s32 def;
    u8 *effScale;
    u8 *dur;

    *flags |= 0x20;
    cost = *(s32 *)(p + 0x39C);
    if (cost != 0) {
        SceneActors_SetHealth(p, *(s16 *)(p + 0x94) + cost);
        *(s32 *)(p + 0x39C) = 0;
    }
    cost = *(s32 *)(p + 0x3A0);
    if (cost != 0) {
        SceneActors_SetHealth(p, *(s16 *)(p + 0x94) - cost);
        *(s32 *)(p + 0x3A0) = 0;
    }
    SceneActors_UpdateEffects(p);
    if (*(u16 *)(p + 0x34E) != 0) {
        *flags |= 4;
        *(u16 *)(p + 0x34E) -= 1;
    }

    if (*flags & 4) {
        *(u16 *)(ov + 6) |= 4;
    } else {
        *(u16 *)(ov + 6) &= ~4;
        Collider_SetPosition(ov, p + 0x260, 0);
    }

    body = p + 0x178;
    *(u8 *)(p + 0x20E) = b48[9];
    if (!(*flags & 8)) {
        Collider_SetPosition(body, p + 0x260, 0);
        *(u8 *)(p + 0x1BA) = b48[9];
        SceneActorGraphics_UpdateVariant(p);
        Collider_Refresh(body);
    }

    b240 = p + 0x240;
    b9E0 = p + 0x9E0;
    if (*flags & 0x10) {
        *(u32 *)(b240 + 8) |= 1;
        ScaledShadow_Disable(p + 0x2A0);
        *(u32 *)(b9E0 + 0x20) &= ~1;
        *(u16 *)(ov + 6) &= ~8;
    } else {
        *(u32 *)(b240 + 8) &= ~1;
        if (*(u8 *)(p + 0xB3) != 0) {
            ScaledShadow_Enable(p + 0x2A0);
            *(u32 *)(b9E0 + 0x20) &= ~1;
        } else {
            ScaledShadow_Enable(p + 0x2A0);
            *(u32 *)(b9E0 + 0x20) |= 1;
        }
        if (*flags & 0x20) {
            *(u16 *)(ov + 6) |= 8;
        } else {
            *(u16 *)(ov + 6) &= ~8;
        }
    }

    if (*(u8 *)(p + 0x399) != 0) {
        *(u8 *)(p + 0x399) -= 1;
        *flags |= 0x80000;
    }

    if (*(s16 *)(p + 0x34C) > 0) {
        *(u16 *)(p + 0x34C) -= 1;
        if (*(u16 *)(p + 0x34C) == 0) {
            res = SceneActorGraphics_GetResourceId(p);
            if (res == 0) {
                dst = p + 0x5C4;
            } else if (res == 1) {
                dst = p + 0x7C4;
            } else {
                dst = p + 0x3C4;
            }
            *(u8 **)(p + 0x288) = dst;
        }
    } else {
        def = SceneActorGraphics_GetDefaultResourceId(p);
        res = *(s32 *)(p + 0x390);
        dst = p + 0x3C4;
        effScale = p + 0x38C;
        dur = p + 0x38F;
        if (*(u8 *)(p + 0x38E) == 1) {
            s32 ip = (s8)*effScale;
            if (ip < (1 << *dur)) {
                *effScale += 1;
                Palette_InterpolateBanks(dst, gPaletteBankData + (def << 5),
                                         gPaletteBankData + (res << 5), (s8)*effScale, *dur);
                *(u8 **)(p + 0x288) = dst;
            }
        } else if (*(u8 *)(p + 0x38E) == 2) {
            if ((s8)*effScale > 0) {
                *effScale -= 1;
                Palette_InterpolateBanks(dst, gPaletteBankData + (def << 5),
                                         gPaletteBankData + (res << 5), (s8)*effScale, *dur);
                *(u8 **)(p + 0x288) = dst;
            } else {
                SceneActorGraphics_SelectResource(p);
            }
        } else if (*(u8 *)(p + 0x38E) == 3) {
            s32 amt = (s8)*effScale;
            Palette_InterpolateBanks(dst, gPaletteBankData + (def << 5),
                                     gPaletteBankData + (res << 5), amt, *dur);
            *(u8 **)(p + 0x288) = dst;
            if (*(u8 *)(p + 0x38D) == 0) {
                *effScale += 1;
                if ((s8)*effScale > 0x1F) {
                    *effScale = 0x20;
                    *(u8 *)(p + 0x38D) = 1;
                }
            } else {
                *effScale -= 1;
                if ((s8)*effScale <= 0) {
                    *effScale = 0;
                    *(u8 *)(p + 0x38D) = 0;
                }
            }
        }
    }

    if (*(u8 *)(p + 0xB3) == 0) {
        CameraTracking_Update(p + 0x9E0);
    } else if (*(u8 *)(p + 0x3B8) != 0 && (*(u32 *)(p + 0xE4) & 0xF) == 0) {
        SceneActors_SpawnAmbientParticlePair(p);
    }

    if (*(s32 *)(p + 0x3C0) > 0) {
        *(s32 *)(p + 0x3C0) -= 1;
        if (*(s32 *)(p + 0x3C0) == 0) {
            SceneActors_StopBackgroundFlash(p);
        }
    }
    *(u32 *)(p + 0xBC) = 0;
    *(u32 *)(p + 0xC0) = 0;
    *(u32 *)(p + 0xE4) += 1;
}
#else
INCLUDE_ASM("asm/nonmatching", sub_080289FC);
#endif
