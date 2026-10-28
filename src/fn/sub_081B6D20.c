#define gEffectSineTable gUnk_086149C4
#include "global.h"
// MATCHING_CONCESSION: register-pinning
struct EffectPosition {
    s16 x, y, z, pad;
};
struct EffectCollider {
    u8 pad[6];
    u16 flags;
    u8 pad8[2];
    u16 contact;
    u8 tail[0x48];
};
struct PooledEffect {
    u8 entered, state;
    u8 pad2[4];
    u16 timer;
    u8 trailIndex, direction;
    u8 padA[10];
    struct EffectPosition position;
    u8 pad1c[0x4c - 0x1c];
    struct EffectPosition origin;
    struct EffectPosition *target;
    struct EffectPosition *targetSource;
    u8 pad5c[0x68 - 0x5c];
    struct EffectCollider collider, healthCollider;
};
extern const s16 gUnk_086149C4[];
extern u32 gRandomIndex;
extern const u8 gUnk_0203B400[];
struct EffectPosition *SceneActors_GetAvailablePosition(void);
s32 Math_Atan2(s32, s32);
void Vec16_Lerp(void *, void *, void *, int, int);
void ActorEffect_UpdatePositionTrail(struct PooledEffect *);
void Collider_SetPosition(void *, void *, u32);
s32 Collider_Refresh(void *);
static inline s32 Scale(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}
void ActorEffect_UpdateApproachLaunch(void *owner, struct PooledEffect *effect, s32 index)
{
    struct EffectPosition delta;
    if (effect->entered) {
        s32 angle;
        struct EffectCollider *c;
        struct EffectPosition *t;
        s32 byte;
        effect->entered = 0;
        c = &effect->healthCollider;
        c->flags |= 4;
        effect->origin = effect->position;
        t = effect->targetSource;
        if (!t)
            t = SceneActors_GetAvailablePosition();
        effect->target = t;
        asm("" ::: "memory");
        angle = Math_Atan2(effect->target->x - effect->position.x,
                           effect->target->z - effect->position.z);
        gRandomIndex = (gRandomIndex + 1) & 1023;
        {
            u32 off = gRandomIndex << 1;
            const u8 *table4;
            table4 = (const u8 *)RANDOM_TABLE_ADDR;
            byte = *(const u8 *)(off + (u32)table4);
        }
        if (byte <= 0x7F)
            effect->direction = angle - 0x40;
        else
            effect->direction = angle + 0x40;
        {
            struct EffectCollider *cc = &effect->collider;
            cc->contact = 1;
            cc++;
            (*(&cc))->contact = 1;
        }
    }
    Vec16_Lerp(&effect->position, &effect->origin, effect->target, effect->timer, 5);
    {
        s32 scale = gUnk_086149C4[(effect->timer << 2) & 255] * 3 >> 5;
        struct EffectPosition *write = &delta;
        u32 direction = effect->direction;
        write->x = Scale(scale * gUnk_086149C4[(direction + 64) & 255]);
        write->y = 0;
        write->z = Scale(scale * gUnk_086149C4[direction]);
    }
    {
        struct EffectPosition *position = &effect->position;
        const struct EffectPosition *read = &delta;
        const struct EffectPosition *first = &delta;
        effect->position.x += first->x;
        position->y += read->y;
        position->z += read->z;
        Collider_SetPosition(&effect->collider, position, 0);
        Collider_SetPosition(&effect->healthCollider, position, 0);
    }
    Collider_Refresh(&effect->collider);
    ActorEffect_UpdatePositionTrail(effect);
    if (effect->timer > 0x1F) {
        effect->state = 2;
        effect->entered = 1;
        effect->timer = 0;
    } else
        effect->timer++;
}
