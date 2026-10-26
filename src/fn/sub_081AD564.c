#include "global.h"
#include "render.h"

// MATCHING_CONCESSION: register-pinning
// confidence=low: applies camera, collision, palette and shake presentation effects.
#define Vanargand_UpdatePresentation sub_081AD564
#define RANDOM_TABLE_ADDR            0x0203B400
struct PresentationVec {
    s16 x, y, z, pad;
};
struct PresentationCollider {
    u8 pad0[6];
    u16 flags;
    u8 pad8[0x54 - 8];
};
struct PresentationAnimation {
    u32 header[2];
    u32 flags;
    u8 padC[0x20 - 0xC];
    struct PresentationVec collisionPosition;
    u8 pad28[0x60 - 0x28];
};
struct VanargandPresentation {
    u8 pad0[0x54];
    struct PresentationVec position;
    u8 pad5C[0x9C - 0x5C];
    u32 flags;
    u8 padA0[0xCC - 0xA0];
    u8 heading;
    u8 padCD[0x110 - 0xCD];
    struct PresentationVec *target;
    u8 pad114[0x18C - 0x114];
    struct PresentationAnimation animation;
    u8 drawable[0x20C - 0x1EC];
    struct PresentationVec blendPosition;
    u8 pad214[0x21C - 0x214];
    u8 paletteState[0x234 - 0x21C];
    u32 shadowFlags;
    u8 pad238[0x418 - 0x238];
    struct PresentationVec cameraPosition;
    u8 pad420[0x4FC - 0x420];
    u32 partFlags;
    u8 pad500[0x56C - 0x500];
    u8 *pair;
    u8 pad570[8];
    u32 impactShake;
    u8 pad57C[0x5A8 - 0x57C];
    u16 collisionValue;
    s16 pauseTimer;
    u8 pad5AC[2];
    s16 restorePaletteTimer;
    s16 shake;
    u8 pad5B2[4];
    u16 selectedHalf;
    u8 pad5B8[0x5C2 - 0x5B8];
    s16 mode;
    u8 pad5C4[4];
    struct PresentationCollider bodyCollider;
    struct PresentationCollider colliders[2];
    struct PresentationCollider healthCollider;
    u8 palette[0x200];
    u8 fadeA[0x200];
    u8 fadeB[0x200];
    u8 fadeC[0xF20 - 0xD18];
    u16 paletteTransition;
    u8 padF22[2];
    u32 variant;
    u8 padF28[0xF34 - 0xF28];
    s32 transition;
    u8 padF38[4];
    u16 cameraShake, cameraMode;
    s32 debugValue;
};
extern u16 gUnk_03005260[];
extern u16 gUnk_03004FC0[];
extern u32 gUnk_03005308;
extern const s16 gUnk_086149C4[];
void SceneActors_UpdateEffects(void *);
void Camera_BlendOffset(void *, void *, s32);
void Camera_SetOffset(s32, void *);
void Camera_RequestShake(s32);
void Collider_SetPosition(void *, void *, s32);
void Collider_Refresh(void *);
void Vanargand_ResetPaletteTransition(void *);
void Vanargand_UpdatePaletteTransition(void *);
s32 ScalarApproach_StepSixteenth(s32, s32);
void ScreenPalette_UpdateState(void *);
static inline s32 Scale(s32 value)
{
    if (value >= 0)
        return value >> 12;
    return -(-value >> 12);
}
static inline void Direction(struct PresentationVec *v, s32 angle, s32 length)
{
    v->x = Scale(gUnk_086149C4[(angle + 64) & 255] * length);
    v->y = 0;
    v->z = Scale(gUnk_086149C4[angle] * length);
}
static inline s32 Blend(s32 t, s32 first, s32 last, s32 duration)
{
    s32 value;
    if (t <= 0)
        return first;
    if (t >= duration)
        return last;
    value = t * ((last - first) << 8) / duration;
    if (value >= 0)
        value >>= 8;
    else
        value = -(-value >> 8);
    return value + first;
}
static inline s32 RandomValue(void)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return *(u16 *)((gUnk_03005308 << 1) + RANDOM_TABLE_ADDR);
}
static inline u32 TestActorFlag(u32 mask, struct VanargandPresentation *self)
{
    return self->flags & mask;
}

static inline void ClearColliderFlag(u32 mask, struct PresentationCollider *c)
{
    c->flags &= ~mask;
}
static inline void StorePalettePair(u32 value)
{
    u16 color = value;
    u16 *palette;
    /* Preserve the packed color before computing the palette addresses. */
    asm("" : "+r"(color));
    palette = gUnk_03004FC0;
    palette[0x5A / 2] = color;
    palette[0x10E / 2] = color;
}
static inline void SetAnimationFlag(struct PresentationAnimation *a)
{
    a->flags |= 1;
}
static inline void ClearAnimationFlag(struct PresentationAnimation *a)
{
    a->flags &= ~1;
}
static inline void ClearActorFlag(u32 mask, struct VanargandPresentation *p)
{
    p->flags &= ~mask;
}
void Vanargand_UpdatePresentation(struct VanargandPresentation *p)
{
    struct VanargandPresentation *self = p;
    struct PresentationCollider *health;

    SceneActors_UpdateEffects(p);
    if (gUnk_03005260[0] & 2) {
        if (gUnk_03005260[1] & 0x40) {
            if (p->debugValue > 1)
                p->debugValue--;
        } else if (gUnk_03005260[1] & 0x80) {
            p->debugValue++;
        }
    }
    if (TestActorFlag(0x200000, self)) {
        switch (p->cameraMode) {
            case 3:
                Camera_SetOffset(8, &self->position);
                break;

            case 1:
                Camera_BlendOffset(self->target, &p->blendPosition, 128);
                break;

            case 2:
                if (p->selectedHalf & 1)
                    Camera_BlendOffset(self->target, p->pair + 0xB4, 128);
                else if (p->selectedHalf & 2)
                    Camera_BlendOffset(self->target, p->pair + 0x2D0, 128);
                break;

            case 4:
                Camera_SetOffset(8, &p->cameraPosition);
                break;

            case 0:

            default:
                Camera_BlendOffset(self->target, &p->cameraPosition, 128);
                break;
        }
    }
    if (p->cameraShake) {
        Camera_RequestShake(p->cameraShake);
        p->cameraShake--;
    }
    if (p->pauseTimer > 0) {
        u32 mask = 4;
        self->flags |= mask;
        p->pauseTimer--;
    }
    if (TestActorFlag(16, self)) {
        SetAnimationFlag(&p->animation);
        ClearActorFlag(0x80000, self);
    } else {
        ClearAnimationFlag(&p->animation);
    }
    if (TestActorFlag(0x80000, self))
        p->shadowFlags &= ~1;
    else
        p->shadowFlags |= 1;
    if (p->impactShake) {
        Camera_RequestShake(12);
        p->impactShake--;
    }
    health = &p->healthCollider;
    if (TestActorFlag(4, self)) {
        p->collisionValue = 0;
        health->flags |= 4;
    } else {
        ClearColliderFlag(4, health);
        Collider_SetPosition(health, &p->animation.collisionPosition, 0);
    }
    health = &p->bodyCollider;
    if (!TestActorFlag(8, self)) {
        s32 i;
        struct PresentationCollider *c;
        Collider_SetPosition(health, &self->position, 0);
        Collider_Refresh(health);
        i = 0;
        do {
            c = &p->colliders[i];
            Collider_SetPosition(c, ((u8 *)p) + 0x17C, 0);
            Collider_Refresh(c);
            i++;
        } while (i <= 1);
    }
    if (p->restorePaletteTimer > 0) {
        if ((--p->restorePaletteTimer) == 0) {
            s32 kind = 0x6F;
            if (!p->variant)
                kind = 0x7F;
            switch (kind) {
                case 0x6F: {
                    s32 r;
                    s32 g;
                    s32 b;
                    s32 t;
                    CpuSet(gPaletteBankData + 0xDE0, p->palette, 0x4000020);
                    CpuSet(p->fadeA, gUnk_03004FC0, 0x4000068);
                    t = p->transition;
                    r = Blend(t, 7, 19, 3);
                    g = Blend(t, 0, 18, 3);
                    b = Blend(t, 0, 19, 3);
                    r |= g << 5;
                    r |= b << 10;
                    {
                        u16 color = r;
                        u16 *palette = gUnk_03004FC0;
                        palette[0x56 / 2] = color;
                        palette[0x10C / 2] = color;
                    }
                    r = Blend(t, 21, 24, 3);
                    g = Blend(t, 0, 29, 3);
                    b = Blend(t, 0, 31, 3);
                    r |= g << 5;
                    r |= b << 10;
                    StorePalettePair((u16)r);
                    break;
                }

                case 0x82:
                    CpuSet(gPaletteBankData + 0x1040, p->palette, 0x4000020);
                    CpuSet(p->fadeB, gUnk_03004FC0, 0x4000068);
                    break;

                case 0x7F:
                    CpuSet(gPaletteBankData + 0xFE0, p->palette, 0x4000018);
                    CpuSet(p->fadeC, gUnk_03004FC0, 0x4000068);
                    break;

                default:
                    CpuSet(gPaletteBankData + 0x3240, p->palette, 0x4000008);
                    break;
            }

            Vanargand_ResetPaletteTransition(p);
        }
    } else if (p->paletteTransition) {
        Vanargand_UpdatePaletteTransition(p);
    }
    if (p->shake > 0) {
        u32 angle = RandomValue();
        s32 product = gUnk_086149C4[(angle + 64) & 255] * (p->shake << 3);
        s32 x = Scale(product);
        s32 z;
        p->animation.collisionPosition.x += x;
        product = gUnk_086149C4[angle & 255] * (p->shake << 3);
        z = Scale(product);
        p->animation.collisionPosition.z += z;
        p->shake--;
    }
    if (p->mode == 1)
        p->partFlags |= 1;
    if (TestActorFlag(0x40000, self)) {
        struct PresentationVec destination;
        Direction(&destination, self->heading, 128);
        destination.x += 1024;
        destination.y += 256;
        destination.z += 1024;
        self->position.x = ScalarApproach_StepSixteenth(self->position.x, destination.x);
        self->position.z = ScalarApproach_StepSixteenth(self->position.z, destination.z);
    }
    ScreenPalette_UpdateState(p->paletteState);
}
