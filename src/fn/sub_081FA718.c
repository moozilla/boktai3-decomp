#include "global.h"
#include "render.h"
struct HalfBits {
    s32 bits : 16;
};
struct Vec {
    s16 x, y, z, pad;
};
struct Collider {
    u8 pad[6];
    s16 flags;
};
struct Sprite {
    u8 pad[9];
    u8 direction;
};
struct Anim {
    u8 pad[8];
    u32 flags;
};
struct Camera {
    u8 pad[0x20];
    u32 flags;
};
struct Boss {
    u8 pad[0x34c];
    struct Anim animation;
    u8 pad358[0x84];
    struct Camera camera;
    u8 pad400[0xa7c];
    s16 flashTimer;
    u8 padE7E[0x56];
    u16 invulnerability;
};
struct Actor {
    u8 pad[0x9c];
    u32 flags;
    u8 padA0[9];
    u8 facing;
    u8 padAA[9];
    u8 mode;
    u8 padB4[8];
    u32 forceX, forceZ;
    u8 padC4[0x20];
    u32 frame;
};
#define H(o) (*(s16 *)(p + (o)))
#define U(o) (*(u16 *)(p + (o)))
#define B(o) (*(u8 *)(p + (o)))
#define S(o) (*(s8 *)(p + (o)))
#define W(o) (*(u32 *)(p + (o)))
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
void CameraTracking_Update(void *);
static inline s32 scale(s32 v)
{
    return v >= 0 ? v >> 12 : -((-v) >> 12);
}
static inline void polar(struct Vec *v, s32 angle, s32 radius)
{
    v->x = scale(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = scale(gUnk_086149C4[angle & 255] * radius);
}
static inline void setFlags(struct Actor *a, u32 mask)
{
    a->flags |= mask;
}
static inline void clearFlags(struct Actor *a, u32 mask)
{
    a->flags &= ~mask;
}
static inline void clearColliderFlags(struct Collider *c, s32 mask)
{
    c->flags &= ~mask;
}
static inline void setAnimFlags(struct Anim *a, u32 mask)
{
    a->flags |= mask;
}
static inline void clearAnimFlags(struct Anim *a, u32 mask)
{
    a->flags &= ~mask;
}
static inline void setCameraFlags(struct Camera *a, u32 mask)
{
    a->flags |= mask;
}
static inline void clearCameraFlags(struct Camera *a, u32 mask)
{
    a->flags &= ~mask;
}
static inline u32 globalFlags(u32 mask)
{
    return gUnk_02000560 & mask;
}
static inline void hide(struct Anim *a)
{
    a->flags |= 1;
}
static inline void show(struct Anim *a)
{
    a->flags &= ~1;
}
static inline void fade(struct Anim *a)
{
    a->flags |= 0x200;
}
static inline void unfade(struct Anim *a)
{
    a->flags &= ~0x200;
}
static inline u16 readHalf(u16 *p)
{
    return *p;
}
static inline u32 flags(struct Actor *a, u32 mask)
{
    return a->flags & mask;
}
void Ratatosk_UpdateCollidersAndPalette(u8 *p)
{
    struct Actor *actor = (struct Actor *)p;
    struct Sprite *sprite = (struct Sprite *)(p + 0x48);
    struct Collider *collider;
    struct Vec pos;
    s32 base, bank, value;
    u8 *palette;
    s32 zero;
    if (((struct Boss *)p)->invulnerability) {
        setFlags(actor, 4);
        ((struct Boss *)p)->invulnerability--;
    }
    if (H(0xed6) > 0)
        setFlags(actor, 4);
    if (!B(0xb3)) {
        collider = (struct Collider *)(p + 0x188);
        if (flags(actor, 4))
            collider->flags |= 4;
        else {
            clearColliderFlags(collider, 4);
            Collider_SetPosition(collider, p + 0x36c, 0);
            {
                u32 direction = sprite->direction;
                B(0x1ca) = direction;
            }
        }
        collider = (struct Collider *)(p + 0x2d8);
        if (flags(actor, 0x800000)) {
            clearColliderFlags(collider, 4);
            Collider_SetPosition(collider, p + 0x36c, 0);
            {
                u32 direction = sprite->direction;
                B(0x31a) = direction;
            }
        } else
            collider->flags |= 4;
        collider = (struct Collider *)(p + 0x284);
        if (flags(actor, 0x800000)) {
            Collider_SetPosition(collider, p + 0x36c, 0);
            {
                u32 direction = sprite->direction;
                B(0x2c6) = direction;
            }
            Collider_Refresh(collider);
        }
        collider = (struct Collider *)(p + 0x1dc);
        if (!flags(actor, 8)) {
            Collider_SetPosition(collider, p + 0x36c, 0);
            {
                u32 direction = sprite->direction;
                B(0x21e) = direction;
            }
            Collider_Refresh(collider);
        }
        collider = (struct Collider *)(p + 0x230);
        if (flags(actor, 0x20000)) {
            polar(&pos, (u8)((actor->facing + 5) << 5), 512);
            {
                struct Vec *v = &pos, *b = (struct Vec *)(p + 0x36c);
                v->x = v->x + b->x;
                v->y = v->y + b->y;
                v->z = v->z + b->z;
            }
            Collider_SetPosition(collider, &pos, 0);
            {
                u32 direction = sprite->direction;
                *((u8 *)collider + 0x42) = direction;
            }
            Collider_Refresh(collider);
            clearFlags(actor, 0x20000);
        }
    } else {
        collider = (struct Collider *)(p + 0x188);
        collider->flags |= 4;
    }
    if (flags(actor, 16)) {
        if (!actor->mode)
            clearCameraFlags(&((struct Boss *)p)->camera, 1);
        hide(&((struct Boss *)p)->animation);
        ScaledShadow_Disable(p + 0x40c);
    } else {
        show(&((struct Boss *)p)->animation);
        if (flags(actor, 0x40000)) {
            if (!actor->mode)
                clearCameraFlags(&((struct Boss *)p)->camera, 1);
            fade(&((struct Boss *)p)->animation);
        } else {
            if (!actor->mode) {
                setCameraFlags(&((struct Boss *)p)->camera, 1);
                Camera_FollowPlayerWithoutCompletion(30);
            }
            unfade(&((struct Boss *)p)->animation);
        }
        ScaledShadow_Enable(p + 0x40c);
        setFlags(actor, 32);
    }
    if (!globalFlags(1)) {
        if (!actor->mode)
            clearCameraFlags(&((struct Boss *)p)->camera, 1);
    }
    if (!actor->mode && flags(actor, 0x1000000))
        clearCameraFlags(&((struct Boss *)p)->camera, 1);
    if (((struct Boss *)p)->flashTimer > 0) {
        fade(&((struct Boss *)p)->animation);
        ((struct Boss *)p)->flashTimer--;
    }
    if (flags(actor, 0x2000000))
        ScaledShadow_Disable(p + 0x40c);
    if (flags(actor, 32))
        ((struct HalfBits *)(p + 0x18e))->bits |= 8;
    else
        U(0x18e) &= ~8;
    if (U(0x1cc)) {
        U(0x1cc)--;
        if ((U(0x1cc) >> 2) & 1)
            hide(&((struct Boss *)p)->animation);
        else
            show(&((struct Boss *)p)->animation);
    }
    if (H(0xed6) > 0) {
        AnimData_SelectPaletteBank(p + 0x478, 402);
        *(u8 **)(p + 0x394) = p + 0x8b8;
        H(0xed6)--;
    } else {
        AnimData_SelectPaletteBank(p + 0x478, 498);
        {
            u8 **dest = (u8 **)(p + 0x394);
            palette = p + 0x4b8;
            *dest = palette;
        }
        zero = 0;
        base = Ratatosk_GetDefaultPaletteIndex(p);
        bank = W(0x4b0);
        if (B(0x4ae) == 1) {
            if (S(0x4ac) < (1 << B(0x4af))) {
                B(0x4ac)++;
                Palette_InterpolateBanks(palette, gPaletteBankData + base * 32,
                                         gPaletteBankData + bank * 32, S(0x4ac), B(0x4af));
            }
        } else if (B(0x4ae) == 2) {
            if (S(0x4ac) > 0) {
                B(0x4ac)--;
                Palette_InterpolateBanks(palette, gPaletteBankData + base * 32,
                                         gPaletteBankData + bank * 32, S(0x4ac), B(0x4af));
            } else
                PaletteEffect_RefreshPalette(p);
        } else if (B(0x4ae) == 3) {
            Palette_InterpolateBanks(palette, gPaletteBankData + base * 32,
                                     gPaletteBankData + bank * 32, S(0x4ac), B(0x4af));
            if (!B(0x4ad)) {
                B(0x4ac)++;
                if (S(0x4ac) > 31) {
                    B(0x4ac) = 32;
                    B(0x4ad) = 1;
                }
            } else {
                B(0x4ac)--;
                if (S(0x4ac) <= 0) {
                    B(0x4ac) = zero;
                    B(0x4ad) = zero;
                }
            }
        } else if (B(0x4ae) == 4) {
            Palette_InterpolateBanks(palette, gPaletteBankData + base * 32, p + 0x6b8, S(0x4ac),
                                     B(0x4af));
            if (!B(0x4ad)) {
                B(0x4ac)++;
                if (S(0x4ac) > 31) {
                    B(0x4ac) = 32;
                    B(0x4ad) = 1;
                }
            } else {
                B(0x4ac)--;
                if (S(0x4ac) <= 0) {
                    B(0x4ac) = zero;
                    B(0x4ad) = zero;
                }
            }
        }
    }
    if (H(0xe74) > 0)
        H(0xe74)--;
    if (H(0xe78) > 0) {
        if (H(0xe78) == 1)
            H(0xe7a) = 0;
        H(0xe78)--;
    }
    {
        s16 *dest = (s16 *)(p + 0xe6e);
        u32 zero = 0;
        *dest = zero;
        actor->frame++;
        actor->forceX = zero;
        actor->forceZ = zero;
        W(0xcbc) = zero;
        W(0xcc0) = zero;
    }
    if (!actor->mode)
        CameraTracking_Update(p + 0x3dc);
}
