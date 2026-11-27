#include "bosses/sabata/sabata_attack_state.h"
#include "effects/particles/scene_particle_burst_api.h"

void Projectile_InitializeParams(struct Task4E01Params *);

void Projectile_SetFlightParams(struct Task4E01Params *, u16, u16, u8, u32);

void Projectile_SetDamageParams(struct Task4E01Params *, u32, u32, u32, u32, u32);

void Projectile_SetCollisionMask(struct Task4E01Params *, u16);

void Projectile_SetTypeAndElement(struct Task4E01Params *, u32, u32);

s32 Projectile_CreateForOwner(struct Task4E01Params *, s16 *, u32);

static inline s32 ScaleQ12(int x)
{
    if (x >= 0)
        return x >> 12;
    return -(-x >> 12);
}

static inline void SetProjectileOrigin(s16 *pos, s32 x, s32 y, s32 z)
{
    s32 zero;
    s16 *dst;
    y += 0xE6;
    zero = 0;
    dst = pos;
    dst[0] = x;
    pos[1] = y;
    pos[2] = z;
    pos[3] = zero;
}

static inline u8 ProjectileDirection(struct SabataAttackActor *actor)
{
    s32 direction = actor->direction8;
    return ((direction + 5) & 7) << 5;
}

void ActorEffect_InitializeOffset(struct SabataAttackActor *actor)
{
    struct SabataAttackActor *volume = actor;
    struct ContactPosition pos;
    s32 x;
    s32 z;
    u32 angle;
    Projectile_InitializeParams(&actor->projectileParameters);
    if (actor->decisionMode == 2)
        Projectile_SetTypeAndElement(&actor->projectileParameters, 2, 6);
    else
        Projectile_SetTypeAndElement(&actor->projectileParameters, 1, 6);
    Projectile_SetFlightParams(&actor->projectileParameters, 0, 0x3C, ProjectileDirection(volume),
                               0x18);
    Projectile_SetDamageParams(&actor->projectileParameters, 0x10, 2, actor->projectilePower, 0x32,
                               0x28);
    Projectile_SetCollisionSize(&actor->projectileParameters, 0x40, 0x64, 0x40);
    Projectile_SetCollisionMask(&actor->projectileParameters, 0x10);
    SetProjectileOrigin((s16 *)&pos, volume->position[0], volume->position[1], volume->position[2]);
    angle = ProjectileDirection(volume);
    x = gUnk_086149C4[(angle + 0x40) & 0xFF];
    z = gUnk_086149C4[angle & 0xFF];
    x = ScaleQ12(0xB4 * x);
    pos.x = pos.x + x;
    z = ScaleQ12(0xB4 * z);
    pos.z = pos.z + z;
    Projectile_CreateForOwner(&actor->projectileParameters, (s16 *)&pos, 1);
}

void SceneActors_StartEightDirectionAnimation(void *, void *, void *, int, int, int, int);

void SceneActors_StartDirectionalAnimation(void *, void *, void *, int, int, int, int);

void ProximityVolume_TranslateBounds(void *, void *);

s32 SabataPosition_TestPath(void *, void *);

s32 Sabata_TestPlayerInAttackArea(void *);

s32 SpriteComponent_GetCollisionStatus(void *);

int Math_Atan2(int, int);

void ActorEffect_SpawnAtAngle(void *, void *, int);

static inline void SetDirection(struct SabataAttackActor *actor, int angle)
{
    actor->direction = angle;
    {
        s32 d = (actor->direction + 0x60) >> 6;
        u32 mask = 3;
        mask &= d;
        actor->direction4 = mask;
    }
    {
        s32 d = (actor->direction + 0x70) >> 5;
        u32 mask = 7;
        mask &= d;
        actor->direction8 = mask;
    }
}

s32 ActorMovement_CanReachTarget(void *);

s32 SabataImpact_TestForwardPoint(void *);

// confidence=low: alternates aim and projectile states, capped at three impacts

void Sabata_UpdateRepeatedShot(struct SabataAttackActor *actor)
{
    struct SabataAttackActor *volume = actor;
    if (actor->restart) {
        actor->restart = 0;
        if (actor->special)
            SetDirection(actor, actor->alternateDirection);
        else
            SetDirection(actor, actor->requestedDirection);
        SceneActors_StartEightDirectionAnimation(actor, &volume->animation, volume->animationState,
                                                 0x134, 1, 1, 1);
        volume->impactCount = 0;
        Sabata_SetFlags(actor, 0x400004);
    }
    switch (actor->phase) {
        case 0:
            if (actor->timer == 0 && ActorMovement_CanReachTarget(volume) == 1)
                SetDirection(actor, actor->requestedDirection);
            actor->timer++;
            SceneActors_StartEightDirectionAnimation(actor, &volume->animation,
                                                     volume->animationState, 0x134, 1, 1, 1);
            if (actor->finished) {
                actor->phase = 1;
                actor->timer = 0;
            }
            break;
        case 1:
            SceneActors_StartEightDirectionAnimation(actor, &volume->animation,
                                                     volume->animationState, 0x139, 1, 1, 1);
            if (actor->timer == 0)
                goto increment;
            if (actor->timer > 10) {
                s32 result = SabataImpact_TestForwardPoint(actor);
                if (result) {
                    u32 phase = 2, zero = 0;
                    actor->phase = phase;
                    actor->timer = zero;
                    ActorEffect_InitializeOffset(volume);
                } else {
                    actor->pending = 1;
                    actor->phase = 3;
                    actor->timer = result;
                }
            } else {
            increment:
                actor->timer++;
            }
            break;
        case 2:
            if (++actor->timer == 1)
                SceneActors_StartEightDirectionAnimation(actor, &volume->animation,
                                                         volume->animationState, 0x13E, 1, 1, 1);
            else if (actor->timer > 1) {
                if (++volume->impactCount > 3) {
                    actor->pending = 1;
                    volume->impactCount = 3;
                    actor->phase = 3;
                    actor->timer = 0;
                } else {
                    actor->phase = 0;
                    actor->timer = 0;
                }
            }
            break;
        case 3:
            if (actor->timer == 0)
                SceneActors_StartEightDirectionAnimation(actor, &volume->animation,
                                                         volume->animationState, 0xF4, 1, 1, 1);
            actor->timer++;
            break;
    }
    if ((u8)Callback_InvokeActor(actor, actor->callback))
        volume->impactCount = 0;
    else
        Sabata_SetFlags(actor, 0x40);
}

// confidence=low: pursues a destination while spawning a rotating effect

void Sabata_UpdatePursuitSpin(struct SabataAttackActor *actor, s32 elapsed)
{
    struct SabataAttackActor *volume = actor;
    if (actor->restart) {
        actor->restart = 0;
        SetDirection(actor, actor->requestedDirection);
        SceneActors_StartEightDirectionAnimation(actor, &actor->animation, actor->animationState,
                                                 0x134, 1, 1, 4);
        if (actor->movementMode == 3)
            ProximityVolume_TranslateBounds(actor->bounds, actor->position);
        else if (actor->movementMode == 1)
            ProximityVolume_TranslateBounds(actor->bounds, actor->position);
        Sabata_SetFlags(actor, 0x400004);
        volume->impactCount = 0;
        Sound_PlaySong(0x2E9);
    }
    if (volume->movementMode == 3) {
        int result = SabataPosition_TestPath(volume, volume->pathPosition);
        if (!result)
            volume->movementMode = result;
    } else if (volume->movementMode == 1) {
        int result = Sabata_TestPlayerInAttackArea(volume);
        if (!result)
            volume->movementMode = result;
    }
    switch (actor->phase) {
        case 0:
            Sabata_SetFlags(actor, 0x400000);
            if (actor->timer == 0)
                SceneActors_StartEightDirectionAnimation(actor, &volume->animation,
                                                         volume->animationState, 0x134, 1, 1, 4);
            if (++actor->timer > 20) {
                actor->phase = 1;
                actor->timer = 0;
            }
            break;
        case 1:
            Sabata_SetFlags(actor, 0x100000);
            if (SpriteComponent_GetCollisionStatus(volume->effect) && !volume->impactCount) {
                volume->impactCount++;
                Sound_PlaySong(0x2E7);
            }
            if (!(elapsed & 3)) {
                struct ContactPosition a, b;
                a.x = 0;
                a.y = 0;
                a.z = 0;
                b.x = 0x20;
                b.y = 0;
                b.z = 0x20;
                SceneParticleBurst_CreateWithPause((const struct ContactPosition *)actor->position,
                                                   &a, &b, 1, 2, 4);
            }
            if (volume->movementMode == 1 || volume->movementMode == 3) {
                int speed = 5;
                int dx = volume->destination[0] - actor->position[0];
                int dz = volume->destination[2] - actor->position[2];
                if (dx * dx + dz * dz <= 25) {
                    actor->velocity[0] = dx;
                    actor->velocity[2] = dz;
                    volume->reached = 1;
                } else {
                    int angle = Math_Atan2(dx, dz) & 0xE0;
                    actor->velocity[0] = ScaleQ12(gUnk_086149C4[(angle + 0x40) & 255] * speed);
                    actor->velocity[2] = ScaleQ12(gUnk_086149C4[angle] * speed);
                }
            }
            actor->timer = (actor->timer + 16) & 255;
            ActorEffect_SpawnAtAngle(volume->effect, actor->position, actor->timer);
            SetDirection(actor, actor->timer + 15);
            SceneActors_StartEightDirectionAnimation(actor, &volume->animation,
                                                     volume->animationState, 0x13E, 1, 1, 1);
            if (elapsed > 96) {
                actor->phase = 2;
                actor->timer = 0;
            }
            if (elapsed <= 31)
                Sabata_SetFlags(actor, 0x400000);
            break;
        case 2:
            volume->impactCount = 0;
            actor->pending = 1;
            SetDirection(actor, actor->requestedDirection);
            SceneActors_StartEightDirectionAnimation(actor, &volume->animation,
                                                     volume->animationState, 0x134, 1, 1, 4);
            break;
    }
    Sabata_SetFlags(actor, 0x40);
    Callback_InvokeActor(actor, actor->callback);
}

void sub_0817AEC8(void *, int);

void Sabata_ConfigureAttackCollider(void *, int, int, int);

void SceneActors_SetParticleControlMode(void *, int);

// confidence=low: decays impact speed, then plays a recovery animation

void Sabata_UpdateImpactSlide(struct SabataAttackActor *actor)
{
    struct SabataAttackActor *volume = actor;
    if (actor->restart) {
        actor->restart = 0;
        SceneActors_StartDirectionalAnimation(actor, &actor->animation, actor->animationState,
                                              0x114, 1, 1, 4);
        actor->phase = 0;
        actor->timer = 0;
        sub_0817AEC8(actor, actor->impactDirection);
        Sabata_ConfigureAttackCollider(actor, 128, ((u8)actor->target->unk2D + 128) & 255, 16);
        SceneActors_SetParticleControlMode(actor, 30);
        Sound_PlaySong(0x50D);
    }
    Sabata_SetFlags(actor, 0x400004);
    if (!(u8)Callback_InvokeActor(actor, actor->callback)) {
        switch (actor->phase) {
            case 0:
                if (actor->speed) {
                    u32 angle = actor->impactDirection;
                    actor->velocity[0] = ScaleQ12(gUnk_086149C4[(angle + 64) & 255] * actor->speed);
                    actor->velocity[2] = ScaleQ12(gUnk_086149C4[angle] * volume->speed);
                    if (volume->slowDown)
                        volume->speed = (volume->speed * 3) >> 2;
                    else
                        volume->speed = (volume->speed * 3) >> 3;
                } else {
                    actor->speed = 0;
                    SceneActors_StartDirectionalAnimation(actor, &actor->animation,
                                                          actor->animationState, 0x116, 1, 1, 1);
                    if (actor->finished) {
                        Sound_PlaySong(0x53E);
                        actor->phase = 1;
                        actor->timer = 0;
                    }
                }
                break;
            case 1:
                actor->pending = 1;
                break;
        }
    }
}

void SceneActors_ConfigureEffect(void *, unsigned, int, int);

static inline void StartTimedAnimation(struct SabataAttackActor *actor, u8 *animation, u8 *state,
                                       s32 sequence)
{
    u16 id = sequence;
    SceneActors_StartEightDirectionAnimation(actor, animation, state, id, 3, 1, 4);
}

static inline void UpdateSequence(struct SabataAttackActor *actor, u32 complete)
{
    if (complete == 0) {
        if (actor->timer == 0) {
            StartTimedAnimation(actor, &actor->animation, actor->animationState, 0x148);
        }
        actor->timer++;
        if (actor->timer > 0xB4) {
            actor->pending = 1;
            actor->phase = 1;
            actor->timer = complete;
        } else if (actor->timer <= 7)
            Sabata_SetFlags(actor, 4);
    }
}

void ActorEffect_UpdateTimedSequence(struct SabataAttackActor *actor)
{
    if (actor->restart) {
        actor->restart = 0;
        actor->phase = 0;
        actor->timer = 0;
        Sound_PlaySong(0x57A);
        Sound_PlaySong(0x13C);
        SceneActors_ConfigureEffect(actor, 5, 0xB4, 0xB4);
    }
    Sabata_SetFlags(actor, 0x400040);
    if ((u8)Callback_InvokeActor(actor, actor->callback) == 0)
        UpdateSequence(actor, actor->phase);
}

u8 *TerrainModifiers_FindCellOverride(u32, int);

static inline int TerrainHeight(s16 *position)
{
    s32 x = position[0] >> 8, z = position[2] >> 8;
    u32 cell;
    u8 *data;
    u32 shape, flags, height;
    if (x < 0 || z < 0 || (u32)x >= gTerrainWidth || (u32)z >= gTerrainHeight)
        cell = 0;
    else
        cell = ((struct EngineMap *)gTerrainMap)->rows[z] + x;
    data = TerrainModifiers_FindCellOverride(cell, 1);
    if (data)
        data += 4;
    else {
        struct EngineMap *map = ((struct EngineMap *)gTerrainMap);
        u32 offset = cell * 4 + 12;
        data = map->cells + offset;
    }
    flags = *data;
    shape = flags >> 4;
    {
        u32 mask = 15;
        mask &= flags;
        height = mask << 8;
    }
    switch (shape) {
        case 1:
            height -= *(u8 *)&position[2];
            break;
        case 2:
            height -= *(u8 *)&position[0];
            break;
    }
    return height;
}

static inline void SetVelocity(s16 *velocity, u32 angle, s32 speed)
{
    s32 x = ScaleQ12(gUnk_086149C4[(angle + 64) & 255] * speed);
    s32 zero = 0;
    velocity[0] = x;
    velocity[1] = zero;
    velocity[2] = ScaleQ12(gUnk_086149C4[angle] * speed);
}

// confidence=low: decays impact velocity and height before landing animations

void Sabata_UpdateKnockbackLanding(struct SabataAttackActor *actor, s32 elapsed)
{
    struct SabataAttackActor *volume = actor;
    if (actor->restart) {
        actor->restart = 0;
        SetDirection(actor, actor->impactDirection + 128);
        SceneActors_StartDirectionalAnimation(actor, &actor->animation, actor->animationState,
                                              0x114, 1, 1, 4);
        if (actor->attackFlags & 1)
            Sound_PlaySong(0x542);
        else
            Sound_PlaySong(0x53D);
        volume->attackFlags &= ~1;
        actor->phase = 0;
        actor->timer = 0;
        volume->height = 300;
    }
    Sabata_SetFlags(actor, 0x400004);
    switch (actor->phase) {
        case 0:
            if (elapsed > 3) {
                SceneActors_StartDirectionalAnimation(actor, &volume->animation,
                                                      volume->animationState, 0x11A, 1, 1, 4);
                actor->phase = 1;
                actor->timer = 0;
            }
            break;
        case 1:
            SetVelocity(actor->velocity, volume->impactDirection, volume->speed);
            actor->position[1] = volume->height + TerrainHeight(actor->position);
            volume->speed = (volume->speed * 3) >> 2;
            if (elapsed > 3) {
                if (volume->height >= elapsed * 8)
                    volume->height -= elapsed * 8;
                else
                    volume->height = 0;
            }
            if (volume->speed <= 19 && volume->height == 0) {
                Sound_PlaySong(0x2A8);
                actor->phase = 2;
                actor->timer = 0;
                SceneActors_StartDirectionalAnimation(actor, &volume->animation,
                                                      volume->animationState, 0x11C, 2, 1, 4);
            }
            break;
        case 2:
            if (++actor->timer > 19) {
                u32 phase = 3;
                actor->phase = phase;
                actor->timer = 0;
            }
            break;
        case 3:
            if (actor->timer == 0) {
                SceneActors_StartDirectionalAnimation(actor, &volume->animation,
                                                      volume->animationState, 0x11E, 2, 1, 4);
                actor->timer++;
            }
            if (actor->finished) {
                actor->pending = 1;
                actor->phase = 4;
                actor->timer = 0;
            }
            break;
        case 4:
            return;
    }
    Sabata_SetFlags(actor, 1);
}
