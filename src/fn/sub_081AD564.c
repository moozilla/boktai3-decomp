#include "global.h"

#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// fix.py best version (reprinted C)
// confidence=low: applies camera, collision, palette and shake presentation effects.
#define Vanargand_UpdatePresentation sub_081AD564
struct PresentationVec {
    s16 x, y, z, pad;
};
struct PresentationCollider {
    u8 pad0[6];
    u16 flags;
    u8 pad8[0x54 - 8];
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
    u32 animation[0x60 / 4];
    u8 drawable[0x21C - 0x1EC];
    u8 paletteState[0x234 - 0x21C];
    u32 shadowFlags;
    u8 pad238[0x2AC - 0x238];
    struct PresentationVec collisionPosition;
    u8 pad2B4[0x418 - 0x2B4];
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
    struct PresentationCollider bodyCollider;
    u8 pad618[4];
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
extern u8 *gUnk_030042E4;
extern u16 gUnk_03004FC0[];
extern u32 gUnk_03005308;
extern u16 gUnk_0203B400[];
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
static inline s32 Blend(s32 t, s32 first, s32 last)
{
    s32 value;
    if (t <= 0)
        return first;
    if (t >= 3)
        return last;
    value = t * ((last - first) << 8) / 3;
    if (value >= 0)
        value >>= 8;
    else
        value = -(-value >> 8);
    return value + first;
}
static inline s32 RandomValue(void)
{
    gUnk_03005308 = (gUnk_03005308 + 1) & 1023;
    return *(u16 *)((gUnk_03005308 << 1) + (u32)gUnk_0203B400);
}
static inline u32 Arg1(u32 k_, struct VanargandPresentation *self)
{
    return self->flags & k_;
}

void Vanargand_UpdatePresentation(struct VanargandPresentation *p)
{
    struct VanargandPresentation *self = p;
    SceneActors_UpdateEffects(p);
    if (gUnk_03005260[0] & 2) {
        if (gUnk_03005260[1] & 0x40) {
            if (p->debugValue > 1)
                p->debugValue--;
        } else if (gUnk_03005260[1] & 0x80) {
            p->debugValue++;
        }
    }
    if (Arg1(0x200000, self)) {
        switch (p->cameraMode) {
            case 3:
                Camera_SetOffset(8, &self->position);
                break;

            case 1:
                Camera_BlendOffset(self->target, &p->collisionPosition, 128);
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
    if (self->flags & 16) {
        p->animation[2] |= 1;
        self->flags &= ~0x80000;
    } else {
        p->animation[2] &= ~1;
    }
    if (self->flags & 0x80000)
        p->shadowFlags &= ~1;
    else
        p->shadowFlags |= 1;
    if (p->impactShake) {
        Camera_RequestShake(12);
        p->impactShake--;
    }
    if (self->flags & 4) {
        p->collisionValue = 0;
        p->healthCollider.flags |= 4;
    } else {
        p->healthCollider.flags &= ~4;
        Collider_SetPosition(&p->healthCollider, &p->collisionPosition, 0);
    }
    if (!(self->flags & 8)) {
        s32 i;
        struct PresentationCollider *c;
        Collider_SetPosition(&p->bodyCollider, &self->position, 0);
        Collider_Refresh(&p->bodyCollider);
        c = p->colliders;
        i = 0;
        do {
            Collider_SetPosition(c, ((u8 *)p) + 0x17C, 0);
            Collider_Refresh(c);
            c++;
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
                    CpuSet(gUnk_030042E4 + 0xDE0, p->palette, 0x4000020);
                    CpuSet(p->fadeA, gUnk_03004FC0, 0x4000068);
                    t = p->transition;
                    r = Blend(t, 7, 19);
                    g = Blend(t, 0, 18);
                    b = Blend(t, 0, 19);
                    r |= g << 5;
                    r |= b << 10;
                    {
                        u16 color = r;
                        u16 *palette = gUnk_03004FC0;
                        palette[0x56 / 2] = color;
                        palette[0x10C / 2] = color;
                    }
                    r = Blend(t, 21, 24);
                    g = Blend(t, 0, 29);
                    b = Blend(t, 0, 31);
                    r |= g << 5;
                    r |= b << 10;
                    {
                        u16 color = r;
                        u16 *palette = gUnk_03004FC0;
                        do {
                            palette[0x5A / 2] = color;
                        } while (0);
                        palette[0x10E / 2] = color;
                    }
                    break;
                }

                case 0x82:
                    CpuSet(gUnk_030042E4 + 0x1040, p->palette, 0x4000020);
                    CpuSet(p->fadeB, gUnk_03004FC0, 0x4000068);
                    break;

                case 0x7F:
                    CpuSet(gUnk_030042E4 + 0xFE0, p->palette, 0x4000018);
                    CpuSet(p->fadeC, gUnk_03004FC0, 0x4000068);
                    break;

                default:
                    CpuSet(gUnk_030042E4 + 0x3240, p->palette, 0x4000008);
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
        p->collisionPosition.x += x;
        product = gUnk_086149C4[angle & 255] * (p->shake << 3);
        z = Scale(product);
        p->collisionPosition.z += z;
        p->shake--;
    }
    if (p->mode == 1)
        p->partFlags |= 1;
    if (self->flags & 0x40000) {
        s16 destination[4];
        s32 angle = self->heading;
        destination[0] = Scale(gUnk_086149C4[(angle + 64) & 255] * 128);
        destination[1] = 0;
        destination[2] = Scale(gUnk_086149C4[angle] * 128);
        destination[0] += 1024;
        destination[1] += 256;
        destination[2] += 1024;
        self->position.x = ScalarApproach_StepSixteenth(self->position.x, destination[0]);
        self->position.z = ScalarApproach_StepSixteenth(self->position.z, destination[2]);
    }
    ScreenPalette_UpdateState(p->paletteState);
}

#else
INCLUDE_ASM("asm/nonmatching", sub_081AD564);
#endif
