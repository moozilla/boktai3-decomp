#include "global.h"
// MATCHING_CONCESSION: register-pinning
// confidence=low: synchronizes colliders, shadow, sound latch and camera flags.
struct V {
    s32 x, y;
};
struct Collider {
    u8 pad0[6];
    s16 flags : 16;
    u8 pad8[12];
    struct V box;
    u8 pad1C[0x42 - 0x1C];
    u8 direction;
    u8 pad43[0x54 - 0x43];
};
struct Actor {
    u8 pad0[0x54];
    u8 position[8];
    u8 pad5C[0x9C - 0x5C];
    u32 flags;
    u8 padA0;
    u8 cameraFlags;
    u8 padA2[6];
    u8 facing;
    u8 padA9[0xBC - 0xA9];
    u32 resetA, resetB;
    u8 padC4[0xE4 - 0xC4];
    u8 angle;
    u8 padE5[0x194 - 0xE5];
    u8 sprite[8];
    u32 spriteFlags;
    u8 pad1A0[0x1AE - 0x1A0];
    u8 blend;
    u8 pad1AF[0x1B4 - 0x1AF];
    u8 collisionPosition[8];
    u8 pad1BC[0x1CE - 0x1BC];
    u16 f1CE;
    u8 pad1D0[12];
    void *f1DC;
    u8 pad1E0[0x1F4 - 0x1E0];
    struct Collider body;
    u8 pad248[0x248 - 0x248];
    struct Collider attacks[2];
    u8 pad2F0[0x312 - 0x2F0];
    s16 timer312;
    u16 invulnA;
    u16 scale;
    u16 invulnB, unused31A, unused31C, unused31E;
    u8 pad320[0x32B - 0x320];
    u8 direction;
    u8 pad32C[0x372 - 0x32C];
    u16 sound;
    u8 pad374[2];
    u16 sway;
    u8 pad378[10];
    u8 cameraState;
    u8 pad383[0x38C - 0x383];
    struct V attackBox;
};
extern u16 gUnk_02000550, gUnk_0200053C;
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
// Remaining differences: branch layout, collider flag insertion, and temporary lifetimes.
#ifdef NONMATCHING
// fuzzy: 86.3%
void Hresvelgr_UpdateCollisionPresentation(struct Actor *p)
{
    struct Actor *p2 = p;
    struct Collider *c;
    register s32 i asm("r5");
    u32 *flags;
    SceneActors_UpdateEffects(p);
    if (p->invulnA) {
        u32 mask = 4;
        *flags |= mask;
        p->invulnA--;
    }
    flags = &p->flags;
    if (p->invulnB) {
        p->invulnB--;
        if ((p->invulnB >> 2) & 1) {
            u32 mask = 16;
            *flags |= mask;
        }
        {
            u32 mask = 4;
            *flags |= mask;
        }
    }
    c = p->attacks;
    if (test(&*flags, 4)) {
        p->unused31E = 0;
        p->unused31C = 0;
        i = 1;
        do {
            c->flags = ((c->flags | 4) | 0) & ~8;
            c++;
        } while (--i >= 0);
    } else {
        c->box = p->attackBox;
        i = 1;
        do {
            {
                s32 mask = ~4;
                c->flags = (c->flags & mask) | 8;
            }
            Collider_SetPosition(c, p->collisionPosition, 0);
            c->direction = p->direction << 6;
            c++;
        } while (--i >= 0);
    }
    c = &p->body;
    if (!test(&*flags, 8)) {
        Collider_SetPosition(c, p->collisionPosition, 0);
        Collider_Refresh(c);
    }
    if (*flags & 0x1010000)
        p->blend = 3;
    else if (*flags & 0x8000000)
        p->blend = 1;
    else
        p->blend = 2;
    if (test(&*flags, 16)) {
        p->spriteFlags |= 1;
        {
            u32 mask = 0x10000;
            *flags |= mask;
        }
    } else
        p->spriteFlags &= ~1;
    if (test(&*flags, 0x10000))
        ScaledShadow_Disable((u8 *)p + 0x3B4);
    else
        ScaledShadow_Enable((u8 *)p + 0x3B4);
    if (test(&*flags, 0x2000000)) {
        gUnk_02000550 = p->sound;
        gUnk_0200053C = p2->facing <= 1 ? 0 : 64;
    } else {
        gUnk_02000550 = 0;
        Sound_PauseChannel12(15);
    }
    if (p->timer312 > 0) {
        p->timer312--;
        if (!p->timer312) {
            p->f1CE = 0x2F0;
            p->f1DC = (u8 *)p + 0x424;
        }
    }
    if ((*flags & 0x200000) && !(gUnk_02000560 & 4)) {
        if (!(p2->cameraFlags & 4)) {
            ColliderActor_UpdateCameraOffset(p);
            goto cameraDone;
        }
    } else if (!(*flags & 2)) {
        p->cameraState = 0;
        Camera_ResetOffset(8);
        goto cameraDone;
    }
    Camera_SetOffset(8, p2->position);
cameraDone:
    if (test(&*flags, 0x400000)) {
        s32 k = shift(gUnk_086149C4[p2->angle] << 7) + 64;
        p->sway += (p->scale * k) / 16;
        if (p->scale <= 15)
            p->scale++;
    } else if (p->scale) {
        s32 k = shift(gUnk_086149C4[p2->angle] << 7) + 64;
        p->sway += (p->scale * k) / 16;
        p->scale--;
    }
    p2->resetA = 0;
    p2->resetB = 0;
}

#else
INCLUDE_ASM("asm/nonmatching", Hresvelgr_UpdateCollisionPresentation);
#endif
