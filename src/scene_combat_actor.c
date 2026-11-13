#include "scene_combat_actor.h"

static inline void SetParticleCallbacks(struct BouncingSceneParticle *particle,
                                        void (*update)(struct BouncingSceneParticle *),
                                        void (*destroy)(struct BouncingSceneParticle *))
{
    particle->update = update;
    particle->destroy = destroy;
}

s32 ArmorEffect_Stop(struct BouncingSceneParticle *s)
{
    s->update = 0;
    if (s->destroy) {
        s->destroy(s);
        s->destroy = NULL;
    }
}

static inline void SetAttackCategories(struct ContactCollider *c)
{
    u32 mask = ~4;
    u32 flags = c->reactionFlags & mask;
    u32 mask2 = ~8;
    u32 categories = c->statRewardFlags & mask2;
    c->reactionFlags = flags | 8;
    c->statRewardFlags = categories | 4;
}

void SceneCombatActor_ResetAttackMode(struct SceneCombatActor *actor)
{
    struct SceneCombatState saved = actor->state;
    struct ContactCollider *a, *b, *c;
    actor->state.common.savedAttack = saved.common.savedAttack;
    actor->state.common.savedDefense = saved.common.savedDefense;
    actor->state.common.phase = 0;
    actor->state.common.value6 = 0;
    actor->state.common.value8 = 0;
    actor->state.common.mode = 0;
    actor->state.common.counter = 0;
    actor->state.common.timer = 420;
    actor->state.common.valueA = 0;
    actor->attack = actor->state.common.savedAttack;
    actor->defense = actor->state.common.savedDefense;
    actor->colliderGroup.colliders[2].unknown3C = actor->defense;
    a = &actor->colliderGroup.colliders[1];
    {
        u32 mask = ~0x200004;
        a->reactionFlags &= mask;
        b = &actor->colliderGroup.colliders[0];
        b->reactionFlags &= mask;
    }
    c = &actor->colliderGroup.colliders[2];
    SetAttackCategories(c);
    b->reactionFlags |= 8;
    a->reactionFlags |= 8;
    actor->flags |= 0x200;
}

static inline void SetDefenseCategories(struct ContactCollider *c)
{
    u32 mask = ~8;
    u32 flags = c->reactionFlags & mask;
    u32 mask2 = ~4;
    u32 categories = c->statRewardFlags & mask2;
    c->reactionFlags = flags | 4;
    c->statRewardFlags = categories | 8;
}

void SceneCombatActor_ResetDefenseMode(struct SceneCombatActor *actor)
{
    struct SceneCombatSnapshot saved = actor->state.common;
    struct ContactCollider *a, *b, *c;
    actor->state.common.savedAttack = saved.savedAttack;
    actor->state.common.savedDefense = saved.savedDefense;
    actor->state.common.phase = 0;
    actor->state.common.value8 = 0;
    actor->state.common.valueA = 0;
    actor->state.common.counter = 0;
    actor->state.common.timer = 0;
    actor->state.common.value6 = 0;
    actor->attack = actor->state.common.savedAttack;
    actor->defense = actor->state.common.savedDefense;
    actor->colliderGroup.colliders[2].unknown3C = actor->defense;
    a = &actor->colliderGroup.colliders[1];
    {
        u32 mask = ~0x400008;
        a->reactionFlags &= mask;
        b = &actor->colliderGroup.colliders[0];
        b->reactionFlags &= mask;
    }
    c = &actor->colliderGroup.colliders[2];
    SetDefenseCategories(c);
    b->reactionFlags |= 4;
    a->reactionFlags |= 4;
    actor->flags |= 0x200;
}

void SceneParticle_UpdateBounce(struct BouncingSceneParticle *actor)
{
    u32 zero;
    actor->sprite.phase += 8;
    zero = 0;
    actor->sprite.position.packed.x += actor->velocity.packed.x;
    actor->sprite.position.packed.y += actor->velocity.packed.y;
    actor->sprite.position.packed.z += actor->velocity.packed.z;
    actor->velocity.packed.y--;
    if (actor->sprite.position.packed.y < actor->floor) {
        if (!actor->bounced) {
            actor->bounced = 1;
            actor->velocity.packed.x >>= 1;
            actor->velocity.packed.y = -(actor->velocity.packed.y >> 1);
            actor->velocity.packed.z >>= 1;
        } else {
            actor->update = (void *)zero;
            if (actor->destroy) {
                Callback_InvokeActor(actor, actor->destroy);
                actor->destroy = (void *)zero;
            }
        }
    }
}

void BouncingEffect_RemoveSprite(struct BouncingSceneParticle *effect)
{
    effect->sprite.flags |= 1;
    Sprite_UnlinkFromList(&effect->sprite, 0);
}

void SceneCombatActor_InitBouncingParticle(struct SceneCombatActor *actor)
{
    struct BouncingParticleSprite *sprite = &actor->looseParticles[1].sprite;
    s32 frame, direction;
    struct BouncingSceneParticle *particle;
    Sprite_Init(sprite, &actor->particleAnimation, 0);
    sprite->flags = 0x202;
    SetParticleCallbacks(&actor->looseParticles[1], SceneParticle_UpdateBounce, BouncingEffect_RemoveSprite);
    actor->looseParticles[1].bounced = 0;
    actor->looseParticles[1].floor = actor->position.values.floor;
    frame = gUnk_0824E6A4[actor->kind];
    direction = ((actor->heading + 0x60) >> 6) & 3;
    switch (direction) {
        case 0:
            sprite->frame = frame;
            break;
        case 1:
            sprite->frame = frame + 1;
            break;
        case 2:
            sprite->frame = frame;
            sprite->flags |= 4;
            break;
        case 3:
            sprite->frame = frame + 1;
            sprite->flags |= 4;
            break;
    }
}

void ActorMarker_InitFacingSprite(struct SceneCombatActor *actor)
{
    struct BouncingParticleSprite *sprite = &actor->facingMarker.sprite;
    s32 direction;
    u32 frame;
    Sprite_Init(sprite, &actor->markerAnimation, 0);
    sprite->flags = 0x202;
    SetParticleCallbacks(&actor->facingMarker, SceneParticle_UpdateBounce, BouncingEffect_RemoveSprite);
    actor->facingMarker.bounced = 0;
    actor->facingMarker.floor = actor->position.values.floor;
    frame = gUnk_0824E6B4[actor->kind];
    direction = ((actor->heading + 0x60) >> 6) & 3;
    switch (direction) {
        case 0:
            sprite->frame = frame;
            break;
        case 1:
            sprite->frame = frame - 4;
            break;
        case 2:
            sprite->frame = frame - 4;
            sprite->flags |= 4;
            break;
        case 3:
            sprite->frame = frame;
            sprite->flags |= 4;
            break;
    }
}

void ActorOverlay_InitSprite(struct SceneCombatActor *actor)
{
    struct BouncingParticleSprite *sprite = &actor->looseParticles[0].sprite;
    Sprite_Init(sprite, &actor->fragmentAnimations[1], 0);
    sprite->flags = 0x202;
    SetParticleCallbacks(&actor->looseParticles[0], SceneParticle_UpdateBounce, BouncingEffect_RemoveSprite);
    actor->looseParticles[0].bounced = 0;
    actor->looseParticles[0].floor = actor->position.values.floor;
    sprite->frame = gUnk_0824E6C4[actor->kind];
}

static inline void SetParticleFrame(struct BouncingParticleSprite *sprite, u32 frame)
{
    sprite->frame = frame;
}

void ActorMarker_InitBouncingParticles(struct SceneCombatActor *actor)
{
    s32 flip = 0;
    s32 side = 0;
    s32 i;
    s32 direction = ((actor->heading + 0x60) >> 6) & 3;
    switch (direction) {
        case 0:
            side = 1;
            break;
        case 1:
            side = 0;
            break;
        case 2:
            side = 0;
            flip = 1;
            break;
        case 3:
            side = 1;
            flip = 1;
            break;
    }
    /* Each direction selects one of the two nine-frame groups. */
    for (i = 0; i < 9; i++) {
        u32 frame = gUnk_0824E6D4[i + side * 9];
        u32 animation;
        switch (actor->kind) {
            case 0:
            case 1:
            case 2:
            case 4:
            case 6:
            case 8:
            case 10:
            case 11:
            case 12:
            case 14:
            default: {
                const u8 *table = gUnk_0824E6E6;
                table += 9;
                animation = table[i];
                break;
            }
            case 3:
            case 5:
            case 7:
            case 9:
            case 13:
            case 15:
                animation = gUnk_0824E6E6[i];
                break;
        }
        Sprite_Init(&actor->fragments[i].sprite, &actor->fragmentAnimations[animation], 0);
        SetParticleFrame(&actor->fragments[i].sprite, frame);
        actor->fragments[i].sprite.flags = 2;
        if (flip)
            actor->fragments[i].sprite.flags = 6;
        if (1 & i)
            actor->fragments[i].sprite.flags |= 0x200;
        else
            actor->fragments[i].sprite.flags |= 0x400;
        SetParticleCallbacks(&actor->fragments[i], SceneParticle_UpdateBounce,
                             BouncingEffect_RemoveSprite);
        actor->fragments[i].bounced = 0;
        actor->fragments[i].floor = actor->position.values.floor;
    }
}

void SceneCombatActor_SpawnBouncingEffect(struct SceneCombatActor *actor)
{
    ActorOverlay_InitSprite(actor);
    actor->looseParticles[0].velocity.packed.z = 0;
    actor->looseParticles[0].velocity.packed.y = 0;
    actor->looseParticles[0].velocity.packed.x = 0;
    actor->looseParticles[0].velocity.packed.y = 30;
    switch (((actor->heading + 0x60) >> 6) & 3) {
        case 0:
            actor->looseParticles[0].sprite.position.packed.x = actor->position.values.x + 0x64;
            actor->looseParticles[0].sprite.position.packed.y = 0x28a + actor->position.values.floor;
            actor->looseParticles[0].sprite.position.packed.z = actor->position.values.z + 0x64;
            actor->looseParticles[0].sprite.phase = 0xf;
            actor->looseParticles[0].velocity.packed.z = 0xf;
            break;
        case 1:
            actor->looseParticles[0].sprite.position.packed.x = actor->position.values.x;
            actor->looseParticles[0].sprite.position.packed.y = 0x28a + actor->position.values.floor;
            actor->looseParticles[0].sprite.position.packed.z = actor->position.values.z + 0xc8;
            actor->looseParticles[0].sprite.phase = 0xf;
            actor->looseParticles[0].velocity.packed.x = -0xf;
            break;
        case 2:
            actor->looseParticles[0].sprite.position.packed.x = actor->position.values.x + 0x96;
            actor->looseParticles[0].sprite.position.packed.y = 0x28a + actor->position.values.floor;
            actor->looseParticles[0].sprite.position.packed.z = actor->position.values.z;
            actor->looseParticles[0].sprite.phase = 0xf5;
            actor->looseParticles[0].velocity.packed.z = -0xf;
            break;
        case 3:
            actor->looseParticles[0].sprite.position.packed.x = actor->position.values.x + 0x64;
            actor->looseParticles[0].sprite.position.packed.y = 0x28a + actor->position.values.floor;
            actor->looseParticles[0].sprite.position.packed.z = actor->position.values.z + 0x64;
            actor->looseParticles[0].sprite.phase = 0xf5;
            actor->looseParticles[0].velocity.packed.x = 0xf;
            break;
    }
}

void SceneCombatActor_SpawnBouncingParticle(struct SceneCombatActor *actor)
{
    SceneCombatActor_InitBouncingParticle(actor);
    actor->looseParticles[1].velocity.packed.z = 0;
    actor->looseParticles[1].velocity.packed.y = 0;
    actor->looseParticles[1].velocity.packed.x = 0;
    actor->looseParticles[1].velocity.packed.y = 30;
    switch (((actor->heading + 0x60) >> 6) & 3) {
        case 0:
            actor->looseParticles[1].sprite.position.packed.x = actor->position.values.x - 0xc8;
            actor->looseParticles[1].sprite.position.packed.y = actor->position.values.floor + 0x64;
            actor->looseParticles[1].sprite.position.packed.z = actor->position.values.z - 0xc8;
            actor->looseParticles[1].sprite.phase = 0;
            actor->looseParticles[1].velocity.packed.z = 0xf;
            break;
        case 1:
            actor->looseParticles[1].sprite.position.packed.x = actor->position.values.x - 0x32;
            actor->looseParticles[1].sprite.position.packed.y = actor->position.values.floor + 0x64;
            actor->looseParticles[1].sprite.position.packed.z = actor->position.values.z - 0xc8;
            actor->looseParticles[1].sprite.phase = 0;
            actor->looseParticles[1].velocity.packed.x = -0xf;
            break;
        case 2:
            actor->looseParticles[1].sprite.position.packed.x = actor->position.values.x - 0xc8;
            actor->looseParticles[1].sprite.position.packed.y = actor->position.values.floor + 0x64;
            actor->looseParticles[1].sprite.position.packed.z = actor->position.values.z - 0x32;
            actor->looseParticles[1].sprite.phase = 0;
            actor->looseParticles[1].velocity.packed.z = -0xf;
            break;
        case 3:
            actor->looseParticles[1].sprite.position.packed.x = actor->position.values.x - 0xc8;
            actor->looseParticles[1].sprite.position.packed.y = actor->position.values.floor + 0x64;
            actor->looseParticles[1].sprite.position.packed.z = actor->position.values.z - 0xc8;
            actor->looseParticles[1].sprite.phase = 0;
            actor->looseParticles[1].velocity.packed.x = 0xf;
            break;
    }
}
