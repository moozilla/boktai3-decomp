#include "global.h"
#include "bosses/ratatosk/ratatosk.h"
#include "player/player.h"
#include "engine/engine.h"
#include "engine/math/scene_vec.h"
#include "engine/video/render.h"
#include "link/wireless/random.h"

void SceneActors_Register(void *, s32);
void SceneActors_Initialize(void *, u32, s32, void *, s32);
void SceneActors_StartUnmirroredAnimation(void *, void *, void *, s32, s32, s32);
void SceneActors_SetHealthLimits(void *, s32, s32, s32);
void SceneActors_SetDamageCallback(void *, void *, void *, s32);
void SceneResource_InitializeHiddenSprite(void *);
void Math_SetMotionPosition(void *, void *);
void *Path_GetDefinition(s32);
void *Path_GetPointData(void *);
void Path_GetPointPosition(void *, void *, s32);
void Ratatosk_InitHurtCollider(void *, s32);
void Ratatosk_InitBodyCollider(void *);
void Ratatosk_InitFrontCollider(void *);
void Ratatosk_InitWideBodyCollider(void *, s32);
void Ratatosk_InitOuterCollider(void *);
void CameraFollow_Initialize(void *, const void *, void *, s32);
void ScaledShadow_InitializeDefault(void *, void *);
void TerrainOverlay_QueueAddActor(void *);
void *HomingProjectile_CreateManager(void);
void *PlayerClone_CreatePool(void);
void *SceneProjectile_CreatePool(void);
void *SunOrb_GetOrCreateManager(void);
void *Ratatosk_CreateAnimatedParticlePool(void);
void *Ratatosk_CreateParticlePool(void);
s32 Mod(s32, s32);
s32 Ratatosk_ApplyQueuedDamage(void *);
void Ratatosk_InitRequestedState(void *);
void Ratatosk_ApplyDamageReaction(void);

static inline const struct ContactPosition *RatatoskInit_GetTargetPosition(struct Ratatosk *base)
{
    return base->playerPosition;
}

static inline void RatatoskInit_SetTarget(struct Ratatosk *base, struct Player *target)
{
    if (target) {
        base->player = target;
        base->playerPosition = (void *)((u8 *)target + 0x30);
    } else {
        base->player = 0;
        base->playerPosition = 0;
    }
}

static inline void RatatoskInit_SetDamageHandler(struct Ratatosk *base, void *callback)
{
    base->actionTryUpdate = callback;
}

static inline void RatatoskInit_SetStateHandler(struct Ratatosk *base, void *callback)
{
    base->actionReset = callback;
}

// confidence=low: Initializes Ratatosk from script dimensions, timing values, paths, palettes, and pools.
s32 Ratatosk_InitializeFromScript(struct Ratatosk *actor, u32 id)
{
    struct Ratatosk *self;
    struct ContactPosition sizes;
    struct ContactPosition motion;
    u32 health, healthExtra;
    u32 attack;
    u32 damage;
    u32 param;
    u32 value;
    s16 *slot;
    u32 facing;
    s32 i;
    u8 *base;
    struct ContactPosition point;
    s32 k;
    s32 introFlag;
    void *pathPoints;

    health = 0x7D0;
    healthExtra = 0;
    attack = 0x1E;
    damage = 0xF;
    facing = 0;
    self = actor;
    SceneActors_Register(actor, 4);
    ZeroFill8(&sizes);
    sizes.x = 0x800;
    sizes.y = 0x200;
    sizes.z = 0x400;

    if (Script_SeekToKeyword(0x65)) {
        actor->firstEventScript = Script_GetValue();
        actor->secondEventScript = Script_GetValue();
    } else {
        actor->firstEventScript = 0;
        actor->secondEventScript = 0;
    }
    if (Script_SeekToKeyword(0x6C)) {
        health = Script_GetValue();
        healthExtra = Script_GetValue();
    }
    if (Script_SeekToKeyword(0x70)) {
        sizes.x = Script_GetValue();
        sizes.y = Script_GetValue();
        sizes.z = Script_GetValue();
    }
    if (Script_SeekToKeyword(0x72)) {
        motion.x = Script_GetValue();
        motion.y = Script_GetValue();
        motion.z = Script_GetValue();
    } else {
        motion.x = 0x800;
        motion.y = 0x200;
        motion.z = 0x800;
    }
    Math_SetMotionPosition(&actor->motionPath, &motion);

    if (Script_SeekToKeyword(0x6D)) {
        for (i = 0; i < 6; i++) {
            actor->points[i].x = Script_GetValue();
            actor->points[i].y = Script_GetValue();
            actor->points[i].z = Script_GetValue();
        }
    }
    if (Script_SeekToKeyword(0x41)) {
        actor->bodyAttack[0] = Script_GetValue();
        actor->bodyAttack[1] = Script_GetValue();
        actor->bodyAttack[2] = Script_GetValue();
        actor->frontAttack[0] = Script_GetValue();
        actor->frontAttack[1] = Script_GetValue();
        value = Script_GetValue();
        slot = &actor->frontAttack[2];
    } else {
        // The defaults are stored through slot. param is reassigned for each store, and
        // value carries the last one to the store after the join, as in the parse branch.
        actor->bodyAttack[0] = attack;
        actor->bodyAttack[1] = damage;
        slot = &actor->bodyAttack[2];
        param = 0x1E;
        *slot = param;
        slot = &actor->frontAttack[0];
        param = 0x28;
        *slot = param;
        slot = &actor->frontAttack[1];
        param = 0x1E;
        *slot = param;
        slot = &actor->frontAttack[2];
        value = 0x1E;
    }
    *slot = value;
    if (Script_SeekToKeyword(0x63)) {
        actor->wideAttack[0] = Script_GetValue();
        actor->wideAttack[1] = Script_GetValue();
        actor->wideAttack[2] = Script_GetValue();
    } else {
        slot = &actor->wideAttack[0];
        param = 5;
        *slot = param;
        slot = &actor->wideAttack[1];
        param = 0;
        *slot = param;
        actor->wideAttack[2] = 0x14;
    }
    if (Script_SeekToKeyword(0x42)) {
        actor->attackEA8[0] = Script_GetValue();
        actor->attackEA8[1] = Script_GetValue();
        actor->attackEA8[2] = Script_GetValue();
    } else {
        slot = &actor->attackEA8[0];
        *slot = 0x1E;
        slot = &actor->attackEA8[1];
        *slot = 0x1E;
        slot = &actor->attackEA8[2];
        *slot = 0x1E;
    }
    if (Script_SeekToKeyword(0x53)) {
        actor->projectileAttack[0] = Script_GetValue();
        actor->projectileAttack[1] = Script_GetValue();
        actor->projectileAttack[2] = Script_GetValue();
        actor->secondaryAttack[0] = Script_GetValue();
        actor->secondaryAttack[1] = Script_GetValue();
        actor->secondaryAttack[2] = Script_GetValue();
        actor->alternateAttack[0] = Script_GetValue();
        actor->alternateAttack[1] = Script_GetValue();
        value = Script_GetValue();
        slot = &actor->alternateAttack[2];
    } else {
        slot = &actor->projectileAttack[0];
        param = 0x1E;
        *slot = param;
        slot = &actor->projectileAttack[1];
        param = 0xA;
        *slot = param;
        slot = &actor->projectileAttack[2];
        param = 0x1E;
        *slot = param;
        slot = &actor->secondaryAttack[0];
        param = 0x32;
        *slot = param;
        slot = &actor->secondaryAttack[1];
        param = 0x1E;
        *slot = param;
        slot = &actor->secondaryAttack[2];
        param = 0x1E;
        *slot = param;
        slot = &actor->alternateAttack[0];
        param = 0x1E;
        *slot = param;
        slot = &actor->alternateAttack[1];
        value = 0x1E;
        *slot = value;
        slot = &actor->alternateAttack[2];
    }
    *slot = value;
    if (Script_SeekToKeyword(0x73)) {
        actor->cloneAttack[0] = Script_GetValue();
        actor->cloneAttack[1] = Script_GetValue();
        actor->cloneAttack[2] = Script_GetValue();
        actor->cloneAttack[3] = Script_GetValue();
        actor->cloneAttack[4] = Script_GetValue();
        value = Script_GetValue();
        slot = &actor->cloneAttack[5];
    } else {
        slot = &actor->cloneAttack[0];
        *slot = 0x1E;
        actor->cloneAttack[1] = 0x1E;
        slot = &actor->cloneAttack[2];
        *slot = 0x1E;
        slot = &actor->cloneAttack[3];
        *slot = 0x1E;
        slot = &actor->cloneAttack[4];
        value = 0x1E;
        *slot = value;
        slot = &actor->cloneAttack[5];
    }
    *slot = value;
    if (Script_SeekToKeyword(0x52)) {
        actor->range[0] = Script_GetValue();
        actor->range[1] = Script_GetValue();
        actor->range[2] = Script_GetValue();
    } else {
        actor->range[0] = 30;
        actor->range[1] = 5;
        actor->range[2] = 30;
    }
    if (Script_SeekToKeyword(0x47)) {
        actor->angleLimit = Script_GetValue();
    } else {
        actor->angleLimit = 20;
    }
    if (Script_SeekToKeyword(0x62)) {
        for (i = 0; i < 6; i++) {
            actor->orbPoints[i].position.x = Script_GetValue();
            actor->orbPoints[i].position.y = Script_GetValue();
            actor->orbPoints[i].position.z = Script_GetValue();
        }
    }
    if (Script_SeekToKeyword(0x44)) {
        introFlag = Script_GetValue();
    } else {
        introFlag = 0;
    }
    if (Script_SeekToKeyword(0x64)) {
        facing = (u8)(((Script_GetValue() << 29) + 0xA0000000u) >> 24);
    }
    if (Script_SeekToKeyword(0x58)) {
        actor->parameterED2 = Script_GetValue();
    } else {
        actor->parameterED2 = 1000;
    }
    if (Script_SeekToKeyword(0x74)) {
        actor->contactValue = Script_GetValue();
    } else {
        actor->contactValue = 0;
    }
    RatatoskInit_SetTarget(self, gPlayers[0]);
    SceneActors_Initialize(self, id, 4, &sizes, facing);
    self->savedAngle = facing;
    actor->displayResource = 0xFB81;
    SceneResource_InitializeHiddenSprite(actor);
    SceneActors_StartUnmirroredAnimation(self, actor->animationData, actor->animation, 7, 1, 0);

    self->locked = Script_GetKeywordValue(0x69, 0);
    if (self->locked == 0) {
        pathPoints = Path_GetPointData(Path_GetDefinition(0));
        for (i = 0; i <= 8; i++) {
            Path_GetPointPosition(&point, pathPoints, i);
            actor->points[i] = point;
            actor->points[i].y = 0x200;
        }
        pathPoints = Path_GetPointData(Path_GetDefinition(1));
        for (i = 0; i <= 8; i++) {
            Path_GetPointPosition(&point, pathPoints, i);
            actor->orbPoints[i].position = point;
            actor->orbPoints[i].position.y = 0x200;
        }
    }
    RatatoskInit_SetDamageHandler(self, Ratatosk_ApplyQueuedDamage);
    RatatoskInit_SetStateHandler(self, Ratatosk_InitRequestedState);
    SceneActors_SetHealthLimits(self, health, health, healthExtra);

    if (!Script_SeekToKeyword(0x70)) {
        self->body.position = actor->points[0];
    }
    Ratatosk_InitHurtCollider(actor, healthExtra);
    Ratatosk_InitBodyCollider(actor);
    Ratatosk_InitFrontCollider(actor);
    Ratatosk_InitWideBodyCollider(actor, healthExtra);
    Ratatosk_InitOuterCollider(actor);

    for (i = 0; i < 16; i++) {
        base = gPaletteBankData + 0x3E40 + i * 0x20;
        CpuSet(base, actor->paletteBase + i * 0x20, 0x4000008);
    }
    for (i = 0; i < 16; i++) {
        base = gPaletteBankData + 0x3E40 + i * 0x20;
        CpuSet(base, actor->paletteNormal + i * 0x20, 0x4000008);
    }
    ((u16 *)actor->paletteNormal)[5] = 0x1F;
    base = gPaletteBankData + 0x3240;
    for (i = 0; i < 16; i++) {
        CpuSet(base, actor->paletteAlternate + i * 0x20, 0x4000008);
    }
    actor->palette = actor->paletteBase;
    actor->blendMode = 0;
    actor->blendBank = 0x1F2;
    actor->blendStep = 0;
    actor->blendShift = 5;
    actor->interval = 60;
    self->flags |= 0x10;

    if (self->locked == 0) {
        Ratatosk_StartAction(self, 0, 0, 1);
    } else if (introFlag == 0) {
        Ratatosk_StartAction(self, 2, 0, 1);
    } else {
        Ratatosk_StartAction(self, 2, 0, 0);
    }
    actor->invulnerability = 0;
    actor->bodyAttack[0] = attack;
    actor->bodyAttack[1] = damage;
    actor->scaleX = 0;
    actor->scaleZ = 0;
    actor->scaleMode = 0;
    actor->projectileCount = 3;
    actor->cloneCount = 3;
    actor->duration = 64;
    actor->savedState = 2;
    actor->duration = 128;
    gRandomIndex = (gRandomIndex + 1) & 0x3FF;
    k = Mod(((u16 *)RANDOM_TABLE_ADDR)[gRandomIndex], 9);
    actor->destination = *(k + actor->points);

    if (self->locked == 0) {
        struct CameraTracking *camera = &actor->camera;
        CameraFollow_Initialize(camera, RatatoskInit_GetTargetPosition(self), &actor->body.position,
                                1);
        CameraTracking_SetRange(camera, 140, 255);
        camera->followMode = 30;
    }
    SceneActors_SetDamageCallback(self, &actor->hurtCollider, Ratatosk_ApplyDamageReaction, 0);
    ScaledShadow_InitializeDefault(actor->shadow, &actor->anchor);
    if (self->locked)
        TerrainOverlay_QueueAddActor(&self->body);
    if (self->locked == 0) {
        actor->homingProjectiles = HomingProjectile_CreateManager();
        if (actor->homingProjectiles == 0)
            return -1;
        actor->playerClones = PlayerClone_CreatePool();
        if (actor->playerClones == 0)
            return -1;
    }
    actor->sceneProjectiles = SceneProjectile_CreatePool();
    if (actor->sceneProjectiles == 0)
        return -1;
    actor->sunOrbs = SunOrb_GetOrCreateManager();
    if (actor->sunOrbs == 0)
        return -1;
    actor->animatedParticles = Ratatosk_CreateAnimatedParticlePool();
    if (actor->animatedParticles == 0)
        return -1;
    actor->particles = Ratatosk_CreateParticlePool();
    if (actor->particles == 0)
        return -1;
    return 0;
}
