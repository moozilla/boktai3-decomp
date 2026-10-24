#include "global.h"

// confidence=low: uses heavy-attack animation records and per-frame collider tables.

/* Arms the heavy-attack collider from animation-frame tables. */

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

struct ActorColliderGroup {
    struct ActorRenderFlags render;
    u8 unknown8[8];
    struct AttackCollider attack;
};

// confidence=low: animation transition arms a stronger attack and updates per-frame collider bounds.
extern const u32 gUnk_0824ECC4[], gUnk_0824ECC8[];
extern const struct HitBox gUnk_0824ED0C[], gUnk_0824ECCC[];
extern const u8 gUnk_0824ED4C[];
extern const struct HitBox gUnk_0824EC04[];
extern const struct HitBox gUnk_0824EC24[];
extern const struct HitBox gUnk_0824EC44[];
extern const struct HitBox gUnk_0824EC64[];
void Armor_SetAnimationFromStart(u32 a, u32 b);
void Collider_SetAttackPowerAndForce(void *, s32, s32);
void Sound_PlaySongChecked(u32 a);

static inline s32 ScaleHeavyAttack(s32 n)
{
    s32 m = 0xaaa * n;
    return m >= 0 ? m >> 12 : -((-m) >> 12);
}

void ActorController_UpdateHeavyAttackCollider(u8 *p)
{
    u8 *s = p + 0xb40;
    s32 st = *s;
    switch (st) {
        case 0: {
            u32 four = 4;
            Armor_SetAnimationFromStart((u32)p, (u32)gUnk_0824ECC4);
            (*s)++;
            p[0xb41] = st;
            ((struct ActorAnimationLayer *)(p + 0x238))->flags |= four;
            ((struct ActorAnimationLayer *)(p + 0x1e4))->flags &= ~four;
            ((struct ActorAnimationLayer *)(p + 0x28c))->flags &= ~four;
            ((struct ActorAnimationLayer *)(p + 0x190))->flags |= four;
            {
                struct ActorRenderFlags *r = (void *)(p + 0x180);
                u32 one = 1;
                r->flags |= one;
            }
            *(u16 *)(p + 0xb32) &= ~0x20;
            break;
        }
        case 1:
            if (p[0xb43]) {
                u32 four = 4;
                struct AttackCollider *c;
                Armor_SetAnimationFromStart((u32)p, (u32)gUnk_0824ECC8);
                (*s)++;
                p[0xb41] = st;
                ((struct ActorAnimationLayer *)(p + 0x238))->flags |= four;
                ((struct ActorAnimationLayer *)(p + 0x1e4))->flags &= ~four;
                ((struct ActorAnimationLayer *)(p + 0x28c))->flags &= ~four;
                ((struct ActorAnimationLayer *)(p + 0x190))->flags &= ~four;
                {
                    struct ActorRenderFlags *r = (void *)(p + 0x180);
                    u32 one = 1;
                    r->flags |= one;
                }
                *(u16 *)(p + 0xb32) &= ~0x20;
                {
                    struct ActorColliderGroup *group = (void *)(p + 0x180);
                    struct AttackCollider *t;
                    u32 mask;
                    t = &group->attack;
                    mask = 0x60000;
                    t->flags |= mask;
                    c = t;
                }
                Collider_SetAttackPowerAndForce(c, (u16)ScaleHeavyAttack(*(s16 *)(p + 0xb26)), 80);
            }
            break;
        case 2:
            if (p[0xd1] & 1)
                p[0xb41] = 1;
            else
                p[0xb41] = 0;
            break;
    }
    if (p[0xb40]) {
        u32 frame = *(u16 *)(p + 0xc8);
        struct AttackCollider *c = (void *)(p + 0x190);
        u32 off = frame << 3;
        c->size = *(const struct HitBox *)(off + (u32)gUnk_0824ED0C);
        c->offset = *(const struct HitBox *)(off + (u32)gUnk_0824ECCC);
        {
            u8 dir = gUnk_0824ED4C[frame];
            p[0x1d2] = dir;
        }
        if (frame == 3 && *(u16 *)(p + 0xc0) == 0)
            Sound_PlaySongChecked(0xbe);
    }
}
