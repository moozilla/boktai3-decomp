#include "effects/bursts/linked_effect.h"

struct LinkedEffectPool *FragmentProjectilePool_GetOrCreate(void);
struct LinkedEffect *FragmentProjectilePool_AllocSprite(struct LinkedEffectPool *);
void ActorEffect_InitColliders(struct LinkedEffect *, s32, s32, s32, s32);

/* Tentative family name; visual identity is not yet established. */
void ActorEffect_Spawn(struct ContactPosition *position, struct ContactPosition *target,
                       s32 attackPower, s32 knockbackPower, s32 attributes, s32 invincibilityFrames,
                       s32 maxSpeed, s32 turnTimer, s32 lifetime, s32 heading)
{
    struct LinkedEffectPool *pool = (struct LinkedEffectPool *)gActorEffectManager;
    struct LinkedEffect *effect;

    if (!pool) {
        pool = FragmentProjectilePool_GetOrCreate();
        if (!pool)
            return;
    }
    effect = FragmentProjectilePool_AllocSprite(pool);
    if (!effect)
        return;
    effect->state = 1;
    effect->unk_174 = 0;
    effect->entered = 1;
    effect->target = target;
    effect->turnTimer = turnTimer;
    effect->lifetime = lifetime;
    effect->speed = 4;
    effect->maxSpeed = maxSpeed;
    effect->delay = 60;
    effect->heading = heading;
    AnimData_Load(effect->graphics, 0xd45e);
    Sprite_Init(effect, effect->graphics, 0);
    Anim_SetSequence(&effect->animation, pool->sequences, 20, 0, 0);
    AnimData_SelectPaletteBank(effect->sprite.graphics, 0x13f);
    effect->sprite.position = *position;
    effect->sprite.flags |= 1;
    effect->sprite.priority = 1;
    ActorEffect_InitColliders(effect, attackPower, knockbackPower, attributes, invincibilityFrames);
}
