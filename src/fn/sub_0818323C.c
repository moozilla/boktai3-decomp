#include "global.h"

/* tentative: arms the attack collider from facing tables with half power */

struct ActorAnimationLayer {
    u16 unknown[3];
    u16 flags;
};
struct ActorRenderFlags {
    u32 unknown, flags;
};
struct HitBox {
    u32 a, b;
};
struct AttackCollider {
    u8 unknown0[0x14];
    struct HitBox offset;
    struct HitBox size;
    u8 unknown24[0x38 - 0x24];
    u32 flags;
};

struct Vec {
    s16 x, y, z, pad;
};
extern u32 gUnk_0824EC84[];
extern const struct HitBox gUnk_0824ECB4;
extern const struct HitBox gUnk_0824EC94[];
extern const u8 gUnk_0824ECBC[];
void Camera_RequestShake(u32);
void SceneParticleBurst_CreateWithPause(void *, u32, const void *, u32, u32, u32);
extern const struct HitBox gUnk_0824EC04[];
extern const struct HitBox gUnk_0824EC24[];
extern const struct HitBox gUnk_0824EC44[];
extern const struct HitBox gUnk_0824EC64[];
void Armor_SetAnimationFromStart(u32 a, u32 b);
void Collider_SetAttackPowerAndForce(void *, s32, s32);
void Sound_PlaySongChecked(u32 a);

static inline s32 Fixed(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}

void ActorController_UpdateMovingAttackCollider(u8 *p)
{
    u8 *s = p + 0xb40;
    u8 st = *s;
    switch (st) {
        case 0: {
            u32 four = 4;
            Armor_SetAnimationFromStart((u32)p, (u32)&gUnk_0824EC84[((p[0xb39] + 0x60) >> 6) & 3]);
            (*s)++;
            p[0xb41] = st;
            Sound_PlaySongChecked(0xb9);
            ((struct ActorAnimationLayer *)(p + 0x238))->flags &= ~four;
            ((struct ActorAnimationLayer *)(p + 0x1e4))->flags &= ~four;
            ((struct ActorAnimationLayer *)(p + 0x28c))->flags &= ~four;
            ((struct ActorAnimationLayer *)(p + 0x190))->flags |= four;
            {
                u32 one = 1;
                ((struct ActorRenderFlags *)(p + 0x180))->flags &= ~one;
            }
            *(u16 *)(p + 0xb32) &= ~0x20;
            break;
        }
        case 1:
            if (p[0xb43]) {
                *s = 255;
                p[0xb41] = st;
            }
            break;
    }
    switch (*(u16 *)(p + 0xc8)) {
        case 9: {
            struct AttackCollider *c;
            const struct Vec *v;
            struct Vec out;
            u8 *facing;
            u32 off, zero;
            p[0xb44] = 0x3f;
            *(u16 *)(p + 0x196) &= ~4;
            {
                struct AttackCollider *t = (void *)(p + 0x190);
                u32 mask = 0x60000;
                t->flags |= mask;
            }
            Collider_SetAttackPowerAndForce(p + 0x190, (u16)Fixed(*(s16 *)(p + 0xb26) << 12), 100);
            c = (void *)(p + 0x190);
            c->size = gUnk_0824ECB4;
            facing = p + 0xb39;
            off = ((*facing + 0x60) >> 6) & 3;
            off <<= 3;
            c->offset = *(const struct HitBox *)(off + (u32)gUnk_0824EC94);
            {
                u8 f = *facing;
                u8 *dest = p + 0x1d2;
                zero = 0;
                *dest = f;
            }
            v = (const void *)(off + (u32)gUnk_0824EC94);
            {
                s16 *dest = (s16 *)&out;
                const s16 *left = (const s16 *)v;
                const s16 *right = (const s16 *)(p + 0x48);
                dest[0] = right[0] + left[0];
                dest[1] = right[1] + left[1];
                dest[2] = right[2] + left[2];
            }
            SceneParticleBurst_CreateWithPause(&out, 0, gUnk_0824ECBC, 15, 1, zero);
            if (*(u16 *)(p + 0xc0) == 0)
                Sound_PlaySongChecked(0xba);
            break;
        }
        case 11: {
            struct AttackCollider *t = (void *)(p + 0x190);
            u32 mask = 0xfff9ffff;
            t->flags &= mask;
        }
            Collider_SetAttackPowerAndForce(p + 0x190, (u16)Fixed(*(s16 *)(p + 0xb26) << 11), 50);
            break;
        case 14: {
            u16 *f = (u16 *)(p + 0x196);
            u32 v = *f;
            u32 four = 4;
            four |= v;
            *f = four;
            break;
        }
    }
    {
        u8 *t = p + 0xb44;
        if (*t) {
            Camera_RequestShake(*t);
            (*t)--;
        }
    }
}
