#include "global.h"
#include "ram.h"

// confidence=low: synchronizes collision, shadow and camera presentation.

struct Vec {
    s16 x, y, z, pad;
};
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
struct Sprite {
    u8 data[8];
    u32 flags;
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
    struct Sprite sprite;
    u8 pad1A0[0x1AE - 0x1A0];
    u8 blend;
    u8 pad1AF[0x1B4 - 0x1AF];
    struct Vec collisionPosition;
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
static inline void SetSpriteVisibility(struct Sprite *sprite, s32 hidden)
{
    if (hidden)
        sprite->flags |= 1;
    else
        sprite->flags &= ~1;
}
static inline void SetSequence(struct Actor *p, u16 sequence, u32 offset)
{
    p->f1CE = sequence;
    p->f1DC = (u8 *)p + offset;
}
void Hresvelgr_UpdateCollisionPresentation(struct Actor *p)
{
    struct Actor *p2 = p;
    struct Collider *c;
    s32 i;
    u32 *flags;
    u32 *resetB, *resetA;
    SceneActors_UpdateEffects(p);
    if (p->invulnA) {
        u32 mask = 4;
        p->flags |= mask;
        p->invulnA--;
    }
    {
        u16 invuln = p->invulnB;
        flags = &p->flags;
        if (invuln) {
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
    }
    c = p->attacks;
    if (test(&*flags, 4)) {
        p->unused31E = 0;
        p->unused31C = 0;
        resetA = &p2->resetA;
        resetB = &p2->resetB;
        for (i = 0; i < 2; i++) {
            c->flags |= 4;
            c->flags &= ~8;
            c++;
        }
    } else {
        c->box = p->attackBox;
        resetA = &p2->resetA;
        resetB = &p2->resetB;
        for (i = 0; i < 2; i++) {
            {
                s32 mask = ~4;
                c->flags = (c->flags & mask) | 8;
            }
            Collider_SetPosition(c, &p->collisionPosition, 0);
            c->direction = p->direction << 6;
            c++;
        }
    }
    c = &p->body;
    if (!test(&*flags, 8)) {
        Collider_SetPosition(c, &p->collisionPosition, 0);
        Collider_Refresh(c);
    }
    if (test(flags, 0x1010000))
        p->blend = 3;
    else if (*flags & 0x8000000)
        p->blend = 1;
    else
        p->blend = 2;
    if (test(&*flags, 16)) {
        SetSpriteVisibility(&p->sprite, 1);
        {
            u32 mask = 0x10000;
            *flags |= mask;
        }
    } else
        SetSpriteVisibility(&p->sprite, 0);
    if (test(&*flags, 0x10000))
        ScaledShadow_Disable((u8 *)p + 0x3B4);
    else
        ScaledShadow_Enable((u8 *)p + 0x3B4);
    if (test(&*flags, 0x2000000)) {
        gPlayerBackgroundState = p->sound;
        gPlayerBackgroundMode = p2->facing <= 1 ? 0 : 64;
    } else {
        gPlayerBackgroundState = 0;
        Sound_PauseChannel12(15);
    }
    if (p->timer312 > 0) {
        p->timer312--;
        if (!p->timer312) {
            SetSequence(p, 0x2F0, 0x424);
        }
    }
    if (test(flags, 0x200000)) {
        u32 cameraMask = 4;
        if (gUnk_02000560 & cameraMask)
            goto defaultCamera;
        if (!(p2->cameraFlags & cameraMask)) {
            ColliderActor_UpdateCameraOffset(p);
            goto cameraDone;
        }
        Camera_SetOffset(8, p2->position);
    } else {
    defaultCamera:
        if (test(flags, 2)) {
            Camera_SetOffset(8, p2->position);
        } else {
            p->cameraState = 0;
            Camera_ResetOffset(8);
        }
    }
cameraDone:
    if (test(&*flags, 0x400000)) {
        s32 k = shift(gUnk_086149C4[p2->angle] << 7) + 64;
        p->collisionPosition.y += (p->scale * k) / 16;
        if (p->scale <= 15)
            p->scale++;
    } else if (p->scale) {
        s32 k = shift(gUnk_086149C4[p2->angle] << 7) + 64;
        p->collisionPosition.y += (p->scale * k) / 16;
        p->scale--;
    }
    *resetA = 0;
    *resetB = 0;
}
