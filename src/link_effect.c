#include "link_effect.h"

extern const s16 gUnk_086149C4[];

s32 Math_Atan2(s32, s32);

static inline s32 ScaleQ12(s32 n)
{
    return n >= 0 ? n >> 12 : -((-n) >> 12);
}

static inline void SetPolarOffset(struct OrbitEffectPosition *v, s32 angle, s32 radius)
{
    v->x = ScaleQ12(gUnk_086149C4[(angle + 64) & 255] * radius);
    v->y = 0;
    v->z = ScaleQ12(gUnk_086149C4[angle & 255] * radius);
}

static inline void AddPosition(struct OrbitEffectPosition *dst, struct OrbitEffectPosition *a,
                               struct OrbitEffectPosition *b)
{
    dst->x = b->x + a->x;
    dst->y = b->y + a->y;
    dst->z = b->z + a->z;
}

void LinkEffect_UpdateEndpoints(struct LinkEffect *effect)
{
    struct OrbitEffectPosition offset;
    s32 angle = Math_Atan2(effect->target->x - gGameState->positionX,
                           effect->target->z - gGameState->positionZ);
    s32 opposite = (angle + 128) & 255;
    SetPolarOffset(&offset, angle, 256);
    offset.y = 128;
    AddPosition(&effect->start, &offset, effect->target);
    SetPolarOffset(&offset, opposite, 256);
    offset.y = 128;
    effect->end.x = gGameState->positionX + offset.x;
    effect->end.y = gGameState->positionY + offset.y;
    effect->end.z = gGameState->positionZ + offset.z;
}

void RisingActorManager_BeginRaise(s32);

void SceneEffect_CreateLinkedEntry(struct LinkEffect *effect)
{
    struct OrbitEffectParams config;
    s32 entry;
    effect->orb = 0xFF;
    effect->otherOrb = -1;
    LinkEffect_UpdateEndpoints(effect);
    RisingActorConfig_Initialize(&config);
    config.unk_07 = 0;
    config.kind = 1;
    config.targetSource = NULL;
    entry = SunOrb_CreateAtPosition(&config, &effect->start);
    if (entry >= 0) {
        RisingActorManager_BeginRaise(entry);
        effect->orb = entry;
    }
    effect->phase |= -1;
}

s32 Mod(s32, s32);

void SlotController_RequestReset(s32);

void SlotController_SetSlotFlag2(s32);

void Projectile_InitParameters(struct SceneProjectileParams *);

void Projectile_SpawnAtPosition(struct SceneProjectileParams *, struct OrbitEffectPosition *);

void Projectile_SetBehaviorMode(struct SceneProjectileParams *, u16);

void Projectile_SetCollisionParameters(struct SceneProjectileParams *, u32, u32, u32, u32, u32);

void Projectile_SetHomingParameters(struct SceneProjectileParams *, u16, u32, u8);

void Projectile_SetLaunchParameters(struct SceneProjectileParams *, u16, u16, u8, u32);

void Projectile_SetScriptProcedure(struct SceneProjectileParams *, u32, u16);

void Projectile_SetSpriteParameters(struct SceneProjectileParams *, u32, u32, u32);

void LinkEffect_UpdateProjectileBurst(struct LinkEffect *effect)
{
    struct SceneProjectileParams projectile;
    struct OrbitEffectPosition position;
    switch (effect->phase) {
        case 0:
            effect->phase = 1;
            effect->emitted = 0;
            RisingActorManager_BeginRaise(effect->orb);
            Sound_PlaySongChecked(0x55B);
            break;
        case 1:
            if (effect->emitted <= 5 && Mod(effect->timer, 15) == 0) {
                u32 random;
                Projectile_InitParameters(&projectile);
                gRandomIndex = (gRandomIndex + 1) & 1023;
                random = *(u8 *)(RANDOM_TABLE_ADDR + gRandomIndex * 2);
                Projectile_SetLaunchParameters(&projectile, 0, 180, random, 8);
                Projectile_SetHomingParameters(
                    &projectile, 120,
                    (u32)SunOrb_GetPositionFromManager(effect->manager, effect->orb), 4);
                Projectile_SetSpriteParameters(&projectile, 0x5291, 0, -1);
                Projectile_SetScriptProcedure(&projectile, 0x3F72, 2);
                Projectile_SetCollisionParameters(&projectile, 0, 0, effect->collision, 15, 30);
                Projectile_SetBehaviorMode(&projectile, 2);
                {
                    struct OrbitEffectPosition *v = &position;
                    *v = *effect->target;
                    v->y += 128;
                    Projectile_SpawnAtPosition(&projectile, v);
                }
                effect->emitted++;
            }
            if (Mod(effect->timer, 300) == 180)
                SlotController_RequestReset(effect->orb);
            if (effect->timer <= 179)
                SlotController_SetSlotFlag2(effect->orb);
            if (effect->timer > 240)
                effect->phase = 255;
            break;
    }
}

u32 SunOrb_FadeOutAll(void *);

void SunOrb_SetEffectTimings(void *, s32, u32, u32);

void SunOrb_SetMotionParams(struct OrbitEffectParams *, u32, u32, u32, u32);

void LinkEffect_UpdateSecondOrb(struct LinkEffect *effect)
{
    struct OrbitEffectParams params;
    switch (effect->phase) {
        case 0: {
            s32 index;
            LinkEffect_UpdateEndpoints(effect);
            RisingActorConfig_Initialize(&params);
            params.unk_07 = 1;
            params.kind = 2;
            params.targetSource = NULL;
            SunOrb_SetMotionParams(&params, 240, 4, 0, 2);
            SunOrb_SetEffectTimings(&params, effect->collision, 15, 30);
            index = SunOrb_CreateAtPosition(&params, &effect->end);
            if (index >= 0) {
                RisingActorManager_BeginRaise(index);
                effect->otherOrb = index;
            }
            effect->phase++;
            break;
        }
        case 1:
            if (effect->timer > 480) {
                SunOrb_FadeOutAll(effect->manager);
                effect->phase = 255;
            }
            break;
    }
}

void SceneBurstEmitter_SetState(struct LinkEffect *effect, u32 v)
{
    u32 z;
    u8 *q = &effect->state;
    z = 0;
    *q = v;
    effect->phase = z;
    effect->timer = z;
}

void SceneBurstEmitter_TransitionIfComplete(struct LinkEffect *effect, u32 v)
{
    if (effect->phase == 0xff) {
        SceneBurstEmitter_SetState(effect, v);
    }
}

s32 SceneEffect_UpdateEntryState(struct LinkEffect *effect)
{
    switch (effect->state) {
        case 0:
        default:
            break;
        case 1:
            SceneEffect_CreateLinkedEntry(effect);
            SceneBurstEmitter_TransitionIfComplete(effect, 2);
            break;
        case 2:
            LinkEffect_UpdateProjectileBurst(effect);
            SceneBurstEmitter_TransitionIfComplete(effect, 3);
            break;
        case 3:
            LinkEffect_UpdateSecondOrb(effect);
            SceneBurstEmitter_TransitionIfComplete(effect, 0);
            break;
    }
    effect->timer++;
    return 0;
}

u32 SceneBurstEmitter_OnDestroy(void)
{
    return 0;
}

u32 SceneProjectile_CreatePool(void *);

u32 SunOrb_GetOrCreateManager(u32);

s32 SceneBurstEmitter_Init(struct LinkEffect *effect)
{
    u32 r = SceneProjectile_CreatePool(effect);
    effect->projectileManager = (void *)r;
    if (r != 0) {
        r = SunOrb_GetOrCreateManager(r);
        effect->manager = (struct OrbitEffectManager *)r;
        if (r != 0) {
            return 0;
        }
    }
    return -1;
}

struct LinkEffect *SceneEffect_CreateEntryTask(u32 resourceId,
                                               const struct OrbitEffectPosition *origin)
{
    struct LinkEffect *effect = Task_Create(8, 0x40);
    if (effect) {
        Task_SetCallbacks(effect, (void (*)(void *))SceneEffect_UpdateEntryState,
                          (void (*)(void *))SceneBurstEmitter_OnDestroy);
        effect->collision = 30;
        effect->target = 0;
        if (SceneBurstEmitter_Init(effect) < 0) {
            Task_Kill(effect);
            return 0;
        }
        effect->collision = resourceId;
        effect->target = origin;
    }
    return effect;
}

void SceneBurstEmitter_RequestCleanup(struct LinkEffect *effect)
{
    if (effect != 0 && effect->state == 0) {
        SceneBurstEmitter_SetState(effect, 3);
    }
}

void DirectionalActor_GetForwardPosition(struct SceneOrbitHazard *hazard,
                                         struct OrbitEffectPosition *out)
{
    const s16 *table = gUnk_086149C4;
    u32 angle = hazard->angle;
    s32 cosine = table[(angle + 64) & 255];
    s32 sine = table[angle];
    out->x = hazard->position.x + ScaleQ12(170 * cosine);
    out->y = hazard->position.y + 80;
    out->z = hazard->position.z + ScaleQ12(170 * sine);
}

void SceneOrbitHazard_SetState(struct SceneOrbitHazard *hazard, s32 v)
{
    hazard->state = v;
    hazard->phase = 0;
    hazard->timer = 0;
}

void SceneOrbitHazard_TransitionIfComplete(struct SceneOrbitHazard *hazard, u32 v)
{
    if (hazard->phase == 0xff) {
        SceneOrbitHazard_SetState(hazard, v);
    }
}

/* Scene orbit hazard identity is tentative. */

void SpriteComponent_ConfigureCollider(void *, s32, s32, s32);

void SpriteComponent_SetFramePosition(void *, s32, struct OrbitEffectPosition *);

void SceneOrbitHazard_UpdateOrbit(struct SceneOrbitHazard *hazard)
{
    struct OrbitEffectPosition position;
    switch (hazard->phase) {
        case 0:
            hazard->angle = 0;
            hazard->position = *hazard->anchor;
            hazard->phase++;
            SpriteComponent_ConfigureCollider(&hazard->effect, hazard->effectParameter, 10, 10);
            break;
        case 1:
            if (hazard->timer > 119)
                hazard->phase = 255;
            break;
    }
    hazard->angle += 16;
    DirectionalActor_GetForwardPosition(hazard, &position);
    SpriteComponent_SetFramePosition(&hazard->effect, hazard->angle, &position);
}

/* Scene orbit hazard identity is tentative. */

void SpriteComponent_Suspend(void *);

static inline s32 SceneOrbitHazard_ScaleVelocity(s32 value)
{
    if (value >= 0)
        return value >> 12;
    else
        return -((-value) >> 12);
}

static inline void SceneOrbitHazard_AddVelocity(struct OrbitEffectPosition *position,
                                                struct OrbitEffectPosition *velocity)
{
    position->x += velocity->x;
    position->y += velocity->y;
    position->z += velocity->z;
}

void SceneOrbitHazard_UpdateTravel(struct SceneOrbitHazard *hazard)
{
    struct OrbitEffectPosition position;
    s32 angle, sine, cosine;
    switch (hazard->phase) {
        case 0:
            angle = Math_Atan2(gGameState->positionX - hazard->anchor->x,
                               gGameState->positionZ - hazard->anchor->z);
            sine = gUnk_086149C4[angle & 255];
            cosine = gUnk_086149C4[(angle + 64) & 255];
            hazard->velocity.x = SceneOrbitHazard_ScaleVelocity(cosine * 15);
            hazard->velocity.y = 0;
            hazard->velocity.z = SceneOrbitHazard_ScaleVelocity(sine * 15);
            hazard->phase++;
            break;
        case 1:
            if (hazard->timer > 179) {
                SpriteComponent_Suspend(&hazard->effect);
                hazard->phase = 255;
                return;
            }
            break;
    }
    hazard->angle += 16;
    SceneOrbitHazard_AddVelocity(&hazard->position, &hazard->velocity);
    DirectionalActor_GetForwardPosition(hazard, &position);
    SpriteComponent_SetFramePosition(&hazard->effect, hazard->angle, &position);
}

s32 SceneEffect_UpdateMovingState(struct SceneOrbitHazard *hazard)
{
    switch (hazard->state) {
        case 0:
        default:
            break;
        case 1:
            SceneOrbitHazard_UpdateOrbit(hazard);
            SceneOrbitHazard_TransitionIfComplete(hazard, 2);
            break;
        case 2:
            SceneOrbitHazard_UpdateTravel(hazard);
            SceneOrbitHazard_TransitionIfComplete(hazard, 0);
            break;
    }
    hazard->timer++;
    return 0;
}

void SpriteComponent_Destroy(void *);

u32 SceneOrbitHazard_OnDestroy(struct SceneOrbitHazard *hazard)
{
    SpriteComponent_Destroy(&hazard->effect);
    return 0;
}

void SpriteComponent_Init(void *);

s32 SceneOrbitHazard_Init(struct SceneOrbitHazard *hazard)
{
    struct LinkSpriteComponent *q = &hazard->effect;
    SpriteComponent_Init(q);
    SpriteComponent_Suspend(q);
    SceneOrbitHazard_SetState(hazard, 0);
    return 0;
}

struct SceneOrbitHazard *SceneEffect_CreateMovingTask(u32 resourceId,
                                                      const struct OrbitEffectPosition *origin)
{
    struct SceneOrbitHazard *hazard = Task_Create(8, 0xE4);
    if (hazard) {
        Task_SetCallbacks(hazard, (void (*)(void *))SceneEffect_UpdateMovingState,
                          (void (*)(void *))SceneOrbitHazard_OnDestroy);
        hazard->effectParameter = 30;
        hazard->anchor = 0;
        if (SceneOrbitHazard_Init(hazard) < 0) {
            Task_Kill(hazard);
            return 0;
        }
        hazard->effectParameter = resourceId;
        hazard->anchor = origin;
    }
    return hazard;
}

void SceneOrbitHazard_Activate(struct SceneOrbitHazard *hazard)
{
    if (hazard != 0 && hazard->state == 0) {
        SceneOrbitHazard_SetState(hazard, 1);
    }
}
