#define gEffectSineTable gUnk_086149C4
#include "global.h"
struct EffectPosition {
    s16 x, y, z, pad;
};
struct EffectCollider {
    u8 pad[6];
    u16 flags;
    u8 tail[0x4c];
};
struct ActorEffect {
    u8 entered, state;
    u8 pad2[4];
    u16 timer;
    u8 trailIndex, direction;
    u16 targetAngle, angle, steeringTimer, speed;
    u8 pad12[2];
    struct EffectPosition position;
    u8 pad1c[0x68 - 0x1c];
    struct EffectCollider collider, healthCollider;
};
extern const s16 gUnk_086149C4[];
void HomingEffect_TurnTowardTarget(struct ActorEffect *);
void ActorEffect_UpdatePositionTrail(struct ActorEffect *);
void Collider_SetPosition(void *, void *, u32);
s32 Collider_Refresh(void *);
static inline s32 Scale(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}
void ActorEffect_UpdateHomingFlight(void *owner, struct ActorEffect *effect, s32 index)
{
    struct EffectPosition delta;
    if (effect->entered) {
        effect->entered = 0;
        {
            struct EffectCollider *c = &effect->healthCollider;
            s32 mask = -5;
            mask &= c->flags;
            c->flags = mask;
        }
        effect->angle = effect->direction << 8;
        effect->steeringTimer = 64;
    }
    if (effect->steeringTimer)
        effect->steeringTimer--;
    HomingEffect_TurnTowardTarget(effect);
    if (effect->steeringTimer > 32) {
        if (effect->speed > 8)
            effect->speed--;
    } else {
        if (effect->speed < 48)
            effect->speed++;
    }
    {
        struct EffectPosition *write = &delta;
        u32 direction = effect->direction;
        u32 speed = effect->speed;
        write->x = Scale(gUnk_086149C4[(direction + 64) & 255] * speed);
        write->y = 0;
        write->z = Scale(gUnk_086149C4[direction] * speed);
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
    if (effect->timer > 149) {
        effect->state = 3;
        effect->entered = 1;
        effect->timer = 0;
    } else
        effect->timer++;
}
