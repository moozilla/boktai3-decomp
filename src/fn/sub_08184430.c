#include "global.h"

/* tentative: 4-state break-apart sequence with particles and camera shake */

struct Layer {
    u16 unknown[3];
    u16 flags;
};
struct RenderFlags {
    u32 unknown, flags;
};
struct Pair {
    u32 a, b;
};
struct Collider {
    u8 pad[0x14];
    struct Pair dir;
    struct Pair size;
    u8 pad2[0x14];
    u32 flags;
};
struct Player {
    u8 pad[0x30];
    s16 x;
    u16 pad2;
    s16 z;
};

extern u32 gUnk_0824EE50[];
extern u32 gUnk_0824EE60[];
extern struct Pair gUnk_0824EE70;
extern struct Pair gUnk_0824EE78;
extern struct Player *gGameState;

void Armor_SetAnimationFromStart(u32 a, u32 b);
void Sound_PlaySongChecked(u32 a);
void ActorPalette_BeginTransition(void *);
void ActorPalette_Reset(void *);
void Anim_UnlinkFromList(void *, u32);
void ScaledShadow_Disable(void *);
void Armor_SpawnDirectionalParticleFan(void *);
void ActorMarker_InitMovingSprite(void *);
void SceneCombatActor_SpawnBouncingEffect(void *);
void ActorMarker_LaunchFragments(void *, u32);
void SpriteColliderGroup_ResetActiveParts(void *);
void Collider_SetAttackPowerAndForce(void *, u32, u32);
u16 Math_Atan2(s32, s32);
void Armor_SpawnRandomDirectionalParticle(void *);
void Camera_RequestShake(u32);

static inline void SetLayers(u8 *p)
{
    u32 four = 4;
    ((struct Layer *)(p + 0x238))->flags |= four;
    ((struct Layer *)(p + 0x1E4))->flags |= four;
    ((struct Layer *)(p + 0x28C))->flags |= four;
    ((struct Layer *)(p + 0x190))->flags &= ~four;
    {
        struct RenderFlags *render = (void *)(p + 0x180);
        u32 one = 1;
        render->flags |= one;
    }
    *(u16 *)(p + 0xB32) &= ~0x20;
}

void Collider_SetAttackPowerAndForce(void *, u32, u32);
static inline s32 FixToInt(s32 x)
{
    return x >= 0 ? x >> 12 : -(-x >> 12);
}

static inline void SetAttack(struct Collider *c, s32 v)
{
    c->flags |= 0x40000;
    Collider_SetAttackPowerAndForce(c, (u16)FixToInt(v), 100);
}

void ActorController_UpdateBreakApartTransition(u8 *p)
{
    u8 *s = p + 0xB40;
    u8 st = *s;

    switch (st) {
        case 0:
            Armor_SetAnimationFromStart((u32)p, (u32)&gUnk_0824EE50[((p[0xB39] + 0x60) >> 6) & 3]);
            (*s)++;
            p[0xB41] = st;
            {
                struct RenderFlags *r = (struct RenderFlags *)(p + 0x180);
                u32 one = 1;
                r->flags |= one;
            }
            Sound_PlaySongChecked(0xC1);
            break;
        case 1:
            if (p[0xB43] != 0) {
                Armor_SetAnimationFromStart((u32)p,
                                            (u32)&gUnk_0824EE60[((p[0xB39] + 0x60) >> 6) & 3]);
                {
                    u32 next = *s + 1;
                    u32 zero = 0;
                    *s = next;
                    p[0xB41] = zero;
                    p[0xB42] = zero;
                }
                ActorPalette_BeginTransition(p);
            }
            break;
        case 2: {
            u8 zero;
            struct Collider *c;
            u8 *t = p + 0xB42;
            if (*t <= 0x1D)
                break;
            {
                u16 *f = (u16 *)(p + 0xB32);
                u32 v = *f;
                u32 m = 0x80;
                zero = 0;
                m |= v;
                *f = m;
            }
            ActorPalette_Reset(p);
            *(u32 *)(p + 0xBC) |= 1;
            Anim_UnlinkFromList(p + 0xB4, 0);
            ScaledShadow_Disable(p + 0x114);
            Armor_SpawnDirectionalParticleFan(p);
            Armor_SpawnDirectionalParticleFan(p);
            Armor_SpawnDirectionalParticleFan(p);
            ActorMarker_InitMovingSprite(p);
            SceneCombatActor_SpawnBouncingEffect(p);
            ActorMarker_LaunchFragments(p, 1);
            SpriteColliderGroup_ResetActiveParts(p + 0xB58);
            (*s)++;
            *t = zero;
            SetLayers(p);
            p[0xB44] = 0x14;
            *(u16 *)(p + 0x196) &= ~4;
            {
                struct Collider *c2 = (struct Collider *)(p + 0x190);
                u32 m = 0x40000;
                c2->flags |= m;
                c = c2;
            }
            Collider_SetAttackPowerAndForce(c, (u16)FixToInt(*(s16 *)(p + 0xB26) << 12), 100);
            c = (struct Collider *)(p + 0x190);
            c->size = gUnk_0824EE78;
            c->dir = gUnk_0824EE70;
            p[0x1D2] =
                Math_Atan2(gGameState->x - *(s16 *)(p + 0x48), gGameState->z - *(s16 *)(p + 0x4C));
        }
        case 3: {
            u8 *t = p + 0xB42;
            if ((*t & 3) == 0) {
                Armor_SpawnRandomDirectionalParticle(p);
                Sound_PlaySongChecked(0x16E);
            }
            if (*t > 0x1D) {
                {
                    u16 four;
                    p[0xB40] = 0xFF;
                    p[0xB41] = 1;
                    {
                        u16 v = *(u16 *)(p + 0x1EA);
                        four = 4;
                        *(u16 *)(p + 0x1EA) = v | four;
                    }
                    *(u16 *)(p + 0x196) |= four;
                }
            }
            break;
        }
    }
    {
        u8 *t = p + 0xB44;
        if (*t != 0) {
            Camera_RequestShake(0x80);
            (*t)--;
        }
    }
}
