#include "engine/video/anim_sprite.h"
#include "bosses/ratatosk/ratatosk.h"
#include "engine/video/render.h"
struct HalfBits {
    s32 bits : 16;
};
extern const s16 gUnk_086149C4[];
extern u32 gUnk_02000560;
void Collider_SetPosition(void *, void *, u32);
void Collider_Refresh(void *);
void ScaledShadow_Disable(void *);
void ScaledShadow_Enable(void *);
void Camera_FollowPlayerWithoutCompletion(s32);
void AnimData_SelectPaletteBank(void *, s32);
s32 Ratatosk_GetDefaultPaletteIndex(void *);
void Palette_InterpolateBanks(void *, void *, void *, s32, s32);
void PaletteEffect_RefreshPalette(void *);
static inline s32 scale(s32 v)
{
    return v >= 0 ? v >> 12 : -((-v) >> 12);
}
static inline void polar(struct ContactPosition *v, s32 angle, s32 radius)
{
    v->x = scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = scale(gUnk_086149C4[angle & 255] * radius);
}
static inline void clearFlags(struct Ratatosk *a, u32 mask)
{
    a->flags &= ~mask;
}
static inline void clearColliderFlags(struct ContactCollider *c, s32 mask)
{
    c->flags = (s16)c->flags & ~mask;
}
static inline u32 globalFlags(u32 mask)
{
    return gUnk_02000560 & mask;
}
static inline void fade(struct AnimSprite *a)
{
    a->flags |= 0x200;
}
static inline void unfade(struct AnimSprite *a)
{
    a->flags &= ~0x200;
}
static inline u32 flags(struct Ratatosk *a, u32 mask)
{
    return a->flags & mask;
}
void Ratatosk_UpdateCollidersAndPalette(struct Ratatosk *p)
{
    struct Ratatosk *actor = p;
    struct SceneActorBody *sprite = &p->body;
    struct ContactCollider *collider;
    struct ContactPosition pos;
    s32 base, bank, value;
    u8 *palette;
    s32 zero;
    if (p->invulnerability) {
        Ratatosk_SetFlags(actor, 4);
        p->invulnerability--;
    }
    if (p->damageTimer > 0)
        Ratatosk_SetFlags(actor, 4);
    if (!p->locked) {
        collider = &p->hurtCollider;
        if (flags(actor, 4))
            collider->flags |= 4;
        else {
            clearColliderFlags(collider, 4);
            Collider_SetPosition(collider, &p->anchor, 0);
            {
                u32 direction = sprite->direction;
                p->hurtCollider.direction = direction;
            }
        }
        collider = &p->wideBodyCollider;
        if (flags(actor, 0x800000)) {
            clearColliderFlags(collider, 4);
            Collider_SetPosition(collider, &p->anchor, 0);
            {
                u32 direction = sprite->direction;
                p->wideBodyCollider.direction = direction;
            }
        } else
            collider->flags |= 4;
        collider = &p->outerCollider;
        if (flags(actor, 0x800000)) {
            Collider_SetPosition(collider, &p->anchor, 0);
            {
                u32 direction = sprite->direction;
                p->outerCollider.direction = direction;
            }
            Collider_Refresh(collider);
        }
        collider = &p->bodyCollider;
        if (!flags(actor, 8)) {
            Collider_SetPosition(collider, &p->anchor, 0);
            {
                u32 direction = sprite->direction;
                p->bodyCollider.direction = direction;
            }
            Collider_Refresh(collider);
        }
        collider = &p->frontCollider;
        if (flags(actor, 0x20000)) {
            polar(&pos, (u8)((actor->facing8 + 5) << 5), 512);
            {
                struct ContactPosition *v = &pos, *b = &p->anchor;
                v->x = v->x + b->x;
                v->y = v->y + b->y;
                v->z = v->z + b->z;
            }
            Collider_SetPosition(collider, &pos, 0);
            {
                u32 direction = sprite->direction;
                collider->direction = direction;
            }
            Collider_Refresh(collider);
            clearFlags(actor, 0x20000);
        }
    } else {
        collider = &p->hurtCollider;
        collider->flags |= 4;
    }
    if (flags(actor, 16)) {
        if (!actor->locked)
            CameraTracking_ClearRequests(&p->camera, 1);
        AnimSprite_Hide((struct AnimSprite *)p->animationData);
        ScaledShadow_Disable(p->shadow);
    } else {
        AnimSprite_Show((struct AnimSprite *)p->animationData);
        if (flags(actor, 0x40000)) {
            if (!actor->locked)
                CameraTracking_ClearRequests(&p->camera, 1);
            fade((struct AnimSprite *)p->animationData);
        } else {
            if (!actor->locked) {
                CameraTracking_SetRequests(&p->camera, 1);
                Camera_FollowPlayerWithoutCompletion(30);
            }
            unfade((struct AnimSprite *)p->animationData);
        }
        ScaledShadow_Enable(p->shadow);
        Ratatosk_SetFlags(actor, 32);
    }
    if (!globalFlags(1)) {
        if (!actor->locked)
            CameraTracking_ClearRequests(&p->camera, 1);
    }
    if (!actor->locked && flags(actor, 0x1000000))
        CameraTracking_ClearRequests(&p->camera, 1);
    if (p->flashTimer > 0) {
        fade((struct AnimSprite *)p->animationData);
        p->flashTimer--;
    }
    if (flags(actor, 0x2000000))
        ScaledShadow_Disable(p->shadow);
    if (flags(actor, 32))
        ((struct HalfBits *)&p->hurtCollider.flags)->bits |= 8;
    else
        p->hurtCollider.flags &= ~8;
    if (p->hurtCollider.responseValue) {
        p->hurtCollider.responseValue--;
        if ((p->hurtCollider.responseValue >> 2) & 1)
            AnimSprite_Hide((struct AnimSprite *)p->animationData);
        else
            AnimSprite_Show((struct AnimSprite *)p->animationData);
    }
    if (p->damageTimer > 0) {
        AnimData_SelectPaletteBank(p->paletteAnimation, 402);
        p->palette = p->paletteAlternate;
        p->damageTimer--;
    } else {
        AnimData_SelectPaletteBank(p->paletteAnimation, 498);
        {
            void **dest = &p->palette;
            palette = p->paletteBase;
            *dest = palette;
        }
        zero = 0;
        base = Ratatosk_GetDefaultPaletteIndex(p);
        bank = p->blendBank;
        if (p->blendMode == 1) {
            if (p->blendStep < (1 << p->blendShift)) {
                p->blendStep++;
                Palette_InterpolateBanks(palette, gPaletteBankData + base * 32,
                                         gPaletteBankData + bank * 32, p->blendStep, p->blendShift);
            }
        } else if (p->blendMode == 2) {
            if (p->blendStep > 0) {
                p->blendStep--;
                Palette_InterpolateBanks(palette, gPaletteBankData + base * 32,
                                         gPaletteBankData + bank * 32, p->blendStep, p->blendShift);
            } else
                PaletteEffect_RefreshPalette(p);
        } else if (p->blendMode == 3) {
            Palette_InterpolateBanks(palette, gPaletteBankData + base * 32,
                                     gPaletteBankData + bank * 32, p->blendStep, p->blendShift);
            if (!p->blendReturning) {
                p->blendStep++;
                if (p->blendStep > 31) {
                    p->blendStep = 32;
                    p->blendReturning = 1;
                }
            } else {
                p->blendStep--;
                if (p->blendStep <= 0) {
                    p->blendStep = zero;
                    p->blendReturning = zero;
                }
            }
        } else if (p->blendMode == 4) {
            Palette_InterpolateBanks(palette, gPaletteBankData + base * 32, p->paletteNormal, p->blendStep,
                                     p->blendShift);
            if (!p->blendReturning) {
                p->blendStep++;
                if (p->blendStep > 31) {
                    p->blendStep = 32;
                    p->blendReturning = 1;
                }
            } else {
                p->blendStep--;
                if (p->blendStep <= 0) {
                    p->blendStep = zero;
                    p->blendReturning = zero;
                }
            }
        }
    }
    if (p->hitTimer > 0)
        p->hitTimer--;
    if (p->damageWindow > 0) {
        if (p->damageWindow == 1)
            p->repeatHits = 0;
        p->damageWindow--;
    }
    p->frameDamage = 0;
    actor->presentationFrames++;
    actor->contactAttack = 0;
    actor->contactFlags = 0;
    p->unknownCBC = 0;
    p->unknownCC0 = 0;
    if (!actor->locked)
        CameraTracking_Update(&p->camera);
}
