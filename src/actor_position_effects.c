#include "actor_position_effects.h"

void SceneActors_InitConvergingParticles(struct ActorState *actor)
{
    struct ConvergingParticleGroup *group = &actor->convergingParticles;
    s32 index;
    group->graphics = AnimData_FindEntry(0x1C1E);
    for (index = 0; index < 4; index++) {
        struct ConvergingParticle *particle = &group->particles[index];
        ParticleSprite_Initialize(particle, group->graphics, 1);
        Effect_SetOrigin(particle, -4, -4);
        Effect_SetPalette(particle, 1);
        particle->priority = 2;
    }
    group->next = 0;
    group->ticks = 0;
}

void SceneActors_RemoveConvergingParticles(struct ActorState *actor)
{
    s32 index;
    struct ConvergingParticle *particle = actor->convergingParticles.particles;
    for (index = 3; index >= 0; index--) {
        Effect_Remove(particle);
        particle++;
    }
}

static inline s32 magnitude(s32 v)
{
    if (v < 0)
        return -v;
    return v;
}

static inline s32 scale(s32 v)
{
    if (v >= 0)
        return v >> 12;
    return -((-v) >> 12);
}

void SceneActors_UpdateConvergingParticles(struct ActorState *actor)
{
    s32 count, index;
    if (!actor->convergingParticles.active)
        return;
    count = 0;
    for (index = 0; index < 4; index++) {
        struct ConvergingParticle *particle = &actor->convergingParticles.particles[index];
        if (particle->active) {
            struct ConvergingParticle *draw = particle;
            if (magnitude(draw->dx) <= 15 && magnitude(draw->dy) <= 15) {
                draw->flags |= 1;
                particle->active = 0;
            } else {
                s32 angle = Math_Atan2(particle->dx, particle->dy);
                particle->dx -= scale(particle->speed * gUnk_086149C4[(angle + 64) & 255]);
                particle->dy -= scale(particle->speed * gUnk_086149C4[angle & 255]);
                draw->position.x = (u16)actor->offsetPoint.x + (u16)particle->dx;
                draw->position.y = (u16)actor->offsetPoint.z + (u16)particle->dy;
                if (++particle->ticks == 4)
                    SpriteDrawable_SelectTileFrame(draw, actor->convergingParticles.graphics, particle->kind + 1);
                count++;
            }
        }
    }
    if (!count)
        actor->convergingParticles.active = 0;
}

static inline s32 random_value(void)
{
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(u16 *)((u8 *)gRandomTable + (gRandomIndex << 1));
}

static inline s32 random_first(void)
{
    u8 *table = gRandomTable;
    gRandomIndex = (gRandomIndex + 1) & 1023;
    return *(u16 *)(table + (gRandomIndex << 1));
}

void SceneActors_SpawnConvergingParticle(struct ActorState *actor, struct ConvergingParticlePoint *pos)
{
    if ((++actor->convergingParticles.ticks & 3) == 0) {
        struct ConvergingParticle *particle = &actor->convergingParticles.particles[actor->convergingParticles.next];
        struct ConvergingParticle *draw = particle;
        s32 angle;
        s32 radius;
        draw->flags &= ~1;
        particle->kind = 4;
        SpriteDrawable_SelectTileFrame(draw, actor->convergingParticles.graphics, 4);
        angle = random_first() & 255;
        radius = (random_first() & 127) + 256;
        do {
            particle->dx = scale(radius * gUnk_086149C4[(angle + 64) & 255]);
        } while (0);
        particle->dy = scale(radius * gUnk_086149C4[angle]);
        draw->position = *pos;
        draw->position.x += (u16)particle->dx;
        draw->position.y += (u16)particle->dy;
        particle->speed = (random_value() & 7) + 28;
        particle->ticks = 0;
        particle->active = 1;
        actor->convergingParticles.active = 1;
        if ((++actor->convergingParticles.next) > 3) {
            actor->convergingParticles.next = 0;
        }
    }
}

void ActorPosition_SetOffsetPoint(struct ActorState *actor)
{
    actor->offsetPoint = actor->body.position;
    actor->offsetPoint.x -= 20;
    actor->offsetPoint.y += 330;
    actor->offsetPoint.z += 60;
}

void SceneActors_EmitRandomBursts(struct ActorState *actor)
{
    struct ActorAnimationResource *graphics = &actor->graphics;
    struct ContactPackedVector size;
    int index;
    for (index = 0; index < 2; index++) {
        int angle, speed;
        int count;
        size.x = 164;
        size.y = 16;
        size.z = 164;
        {
            union RandomTableView table = {gRandomTable};
            angle = (u8)ParticleRandom_ReadDirectSample(table.samples);
            count = (ParticleRandom_ReadDirectSample(table.samples) & 3) + 2;
            speed = (ParticleRandom_ReadDirectSample(table.samples) & 3) + 20;
        }
        SparkParticle_Spawn(graphics->eventScripts, 3, &size, angle, count, speed, 2, 2);
    }
}

static inline void ActorPosition_StartReaction(struct ActorState *actor, int id, int zero)
{
    int one = 1;
    actor->active = one;
    actor->phase = zero;
    actor->variant = id;
    actor->timer = zero;
    actor->triggered = one;
}

int ActorPosition_BeginReaction(struct ActorState *actor, int kind, int damage)
{
    struct SceneActorBody *body = &actor->body;
    int initialPhase;
    void (*callback)(void *);
    u32 requestedState;
    switch (kind) {
        case 0: {
            int state;
            actor->vitality += damage;
            actor->lastDamage = damage;
            SceneVolume_BeginPhase(actor, 2);
            initialPhase = 0;
            ActorPosition_StartReaction(actor, initialPhase, 0);
            callback = SceneEventActor_UpdateJitter;
            state = 0xb;
            actor->controlPhase = state;
            actor->stateCallback = callback;
            actor->stateTicks = kind;
            actor->changed = 1;
            actor->stateReset = 0;
            actor->pendingAction = kind;
            actor->state = state;
            break;
        }
        case 2:
            ActorPosition_StartReaction(actor, 1, 0);
            {
                int rq = 0xc;
                actor->state = rq;
            }
            {
                int angle = body->direction + 0x80;
                actor->direction = angle;
            }
            break;
        case 1: {
            int angle = ((struct ActorDamageSource *)actor->damageTarget)->direction;
            u8 *dst = &actor->direction;
            int zero = 0;
            *dst = angle;
            {
                u32 mask = 2;
                actor->flags |= mask;
            }
            ActorPosition_StartReaction(actor, 3, zero);
            requestedState = 0xd;
            actor->state = requestedState;
        } break;
    }
    return 1;
}

s32 ActorPosition_TestPath(struct ActorState *actor, struct ContactPosition *destination)
{
    if (Nav_IsPositionInMesh(actor->bounds, &actor->body.position, destination) &&
        NavMesh_FindRoute(actor->bounds, &actor->body.position, destination))
        return 1;
    return 0;
}
