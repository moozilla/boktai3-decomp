#include "global.h"
#include "scene_actor_init_actor.h"
#include "engine.h"
#include "scene_vec.h"
#include "render.h"

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
extern void *gUnknown_02000580;
s32 sub_081F6428(void *);
void Ratatosk_InitRequestedState(void *);
void Ratatosk_ApplyDamageReaction(void);

struct RatatoskCamera {
    u8 unknown0[0x10];
    u32 angle, distance;
    u8 unknown18[0x14];
    u32 duration;
};
struct RatatoskOrbSlot {
    struct ContactPosition position;
    u8 unknown8[0x10];
};
struct RatatoskInitBase {
    u8 unknown0[0x48 - 0x0];
    struct SceneActorBody body;
    u8 unknown90[0x9C - 0x90];
    u32 flags;
    u8 unknownA0[0xAC - 0xA0];
    u8 animationMode;
    u8 animationPhase;
    u8 animationEntry;
    u8 unknownAF[0xB0 - 0xAF];
    u8 animationChanged;
    u8 unknownB1[0xB3 - 0xB1];
    u8 specialMode;
    u8 unknownB4[0xC4 - 0xB4];
    u32 animationTimer;
    u8 unknownC8[0xE8 - 0xC8];
    u32 eventScriptA;
    u32 eventScriptB;
    u8 unknownF0[0x104 - 0xF0];
    void *damageCallback;
    void *stateCallback;
    const void *target;
    const void *targetPosition;
    u8 savedFacing;
    u8 unknown115[0x178 - 0x115];
};

struct RatatoskInit {
    struct RatatoskInitBase base;
    u8 unknown178[0x188 - 0x178];
    u8 damageContext[1];
    u8 unknown189[0x32C - 0x189];
    u8 animation[0x20];
    u8 animationData[0x20];
    struct ContactPosition anchor;
    u8 unknown374[0x394 - 0x374];
    void *palette;
    u8 unknown398[0x3AC - 0x398];
    u8 motion[0x30];
    struct RatatoskCamera camera;
    u8 shadow[0x40];
    u8 unknown44C[0x494 - 0x44C];
    u32 displayResource;
    u8 unknown498[0x4AC - 0x498];
    u8 blendMode;
    u8 unknown4AD[0x4AE - 0x4AD];
    u8 blendEnabled;
    u8 blendShift;
    u32 blendResource;
    u8 unknown4B4[0x4B8 - 0x4B4];
    u8 paletteBase[0x200];
    u8 paletteNormal[0x200];
    u8 paletteAlternate[0x200];
    u8 unknownAB8[0xCCC - 0xAB8];
    struct ContactPosition points[9];
    u8 unknownD14[0xD1C - 0xD14];
    struct RatatoskOrbSlot orbSlots[9];
    u8 unknownDF4[0xE52 - 0xDF4];
    u16 scaleX;
    u16 scaleZ;
    u8 unknownE56[0xE58 - 0xE56];
    u16 scaleStep;
    u8 unknownE5A[0xE5C - 0xE5A];
    struct ContactPosition destination;
    u8 unknownE64[0xE6C - 0xE64];
    u16 motionScale;
    u8 unknownE6E[0xE9C - 0xE6E];
    u16 hurtX;
    u16 hurtY;
    u16 hurtZ;
    u16 hurtAttack;
    u16 hurtDamage;
    u16 hurtKind;
    u16 bodyX;
    u16 bodyY;
    u16 bodyZ;
    u16 wideX;
    u16 wideY;
    u16 wideZ;
    u16 projectileX;
    u16 projectileY;
    u16 projectileZ;
    u16 secondaryX;
    u16 secondaryY;
    u16 secondaryZ;
    u16 alternateX;
    u16 alternateY;
    u16 alternateZ;
    u16 cloneX;
    u16 cloneY;
    u16 cloneZ;
    u16 cloneSecondaryX;
    u16 cloneSecondaryY;
    u16 cloneSecondaryZ;
    u16 parameterED2;
    u16 parameterED4;
    u8 unknownED6[0xEDA - 0xED6];
    u16 projectileCount;
    u16 cloneCount;
    u8 unknownEDE[0xEE6 - 0xEDE];
    u16 parameterEE6;
    u8 unknownEE8[0xEF0 - 0xEE8];
    u32 savedState;
    u8 unknownEF4[0xEFC - 0xEF4];
    u16 rangeX;
    u16 rangeY;
    u16 rangeZ;
    u8 unknownF02[0xF04 - 0xF02];
    u32 delay;
    u8 unknownF08[0xF0C - 0xF08];
    u16 parameterF0C;
    u8 unknownF0E[0xF18 - 0xF0E];
    void *homingProjectiles;
    void *playerClones;
    void *particles;
    void *animatedParticles;
    void *sceneProjectiles;
    void *sunOrbs;
};

static inline const void *RatatoskInit_GetTargetPosition(struct RatatoskInitBase *base)
{
    return base->targetPosition;
}

static inline void RatatoskInit_SetTarget(struct RatatoskInitBase *base, void *target)
{
    if (target) {
        base->target = target;
        base->targetPosition = (u8 *)target + 0x30;
    } else {
        base->target = 0;
        base->targetPosition = 0;
    }
}
static inline void RatatoskInit_SetDamageHandler(struct RatatoskInitBase *base, void *callback)
{
    base->damageCallback = callback;
}
static inline void RatatoskInit_SetStateHandler(struct RatatoskInitBase *base, void *callback)
{
    base->stateCallback = callback;
}
static inline void RatatoskInit_ConfigureAnimation(struct RatatoskInitBase *base, u8 mode, u8 entry)
{
    base->animationMode = mode;
    base->animationPhase = 0;
    base->animationEntry = entry;
    base->animationTimer = 0;
    base->animationChanged = 1;
}

static inline void RatatoskInit_SetCameraOrbit(struct RatatoskCamera *camera, u32 angle,
                                               u32 distance)
{
    camera->angle = angle;
    camera->distance = distance;
}

// confidence=low: Initializes Ratatosk from script dimensions, timing values, paths, palettes, and pools.
#define Ratatosk_InitializeFromScript sub_081FAE0C
s32 Ratatosk_InitializeFromScript(struct RatatoskInit *actor, u32 id)
{
    struct RatatoskInitBase *self;
    struct ContactPosition sizes;
    struct ContactPosition motion;
    u32 hpA, hpB;
    u32 defaultHurtX;
    u32 defaultHurtY;
    u32 param;
    u32 value;
    u16 *slot;
    u32 facing;
    s32 i;
    u8 *base;
    struct ContactPosition pv;
    s32 k;
    s32 animationGate;
    void *pdata;

    hpA = 0x7D0;
    hpB = 0;
    defaultHurtX = 0x1E;
    defaultHurtY = 0xF;
    facing = 0;
    self = &actor->base;
    SceneActors_Register(actor, 4);
    ZeroFill8(&sizes);
    sizes.x = 0x800;
    sizes.y = 0x200;
    sizes.z = 0x400;

    if (Script_SeekToKeyword(0x65)) {
        actor->base.eventScriptA = Script_GetValue();
        actor->base.eventScriptB = Script_GetValue();
    } else {
        actor->base.eventScriptA = 0;
        actor->base.eventScriptB = 0;
    }
    if (Script_SeekToKeyword(0x6C)) {
        hpA = Script_GetValue();
        hpB = Script_GetValue();
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
    Math_SetMotionPosition(actor->motion, &motion);

    if (Script_SeekToKeyword(0x6D)) {
        for (i = 0; i < 6; i++) {
            actor->points[i].x = Script_GetValue();
            actor->points[i].y = Script_GetValue();
            actor->points[i].z = Script_GetValue();
        }
    }
    if (Script_SeekToKeyword(0x41)) {
        actor->hurtX = Script_GetValue();
        actor->hurtY = Script_GetValue();
        actor->hurtZ = Script_GetValue();
        actor->hurtAttack = Script_GetValue();
        actor->hurtDamage = Script_GetValue();
        value = Script_GetValue();
        slot = &actor->hurtKind;
    } else {
        // The defaults are stored through slot. param is reassigned for each store, and
        // value carries the last one to the store after the join, as in the parse branch.
        actor->hurtX = defaultHurtX;
        actor->hurtY = defaultHurtY;
        slot = &actor->hurtZ;
        param = 0x1E;
        *slot = param;
        slot = &actor->hurtAttack;
        param = 0x28;
        *slot = param;
        slot = &actor->hurtDamage;
        param = 0x1E;
        *slot = param;
        slot = &actor->hurtKind;
        value = 0x1E;
    }
    *slot = value;
    if (Script_SeekToKeyword(0x63)) {
        actor->wideX = Script_GetValue();
        actor->wideY = Script_GetValue();
        actor->wideZ = Script_GetValue();
    } else {
        slot = &actor->wideX;
        param = 5;
        *slot = param;
        slot = &actor->wideY;
        param = 0;
        *slot = param;
        actor->wideZ = 0x14;
    }
    if (Script_SeekToKeyword(0x42)) {
        actor->bodyX = Script_GetValue();
        actor->bodyY = Script_GetValue();
        actor->bodyZ = Script_GetValue();
    } else {
        slot = &actor->bodyX;
        *slot = 0x1E;
        slot = &actor->bodyY;
        *slot = 0x1E;
        slot = &actor->bodyZ;
        *slot = 0x1E;
    }
    if (Script_SeekToKeyword(0x53)) {
        actor->projectileX = Script_GetValue();
        actor->projectileY = Script_GetValue();
        actor->projectileZ = Script_GetValue();
        actor->secondaryX = Script_GetValue();
        actor->secondaryY = Script_GetValue();
        actor->secondaryZ = Script_GetValue();
        actor->alternateX = Script_GetValue();
        actor->alternateY = Script_GetValue();
        value = Script_GetValue();
        slot = &actor->alternateZ;
    } else {
        slot = &actor->projectileX;
        param = 0x1E;
        *slot = param;
        slot = &actor->projectileY;
        param = 0xA;
        *slot = param;
        slot = &actor->projectileZ;
        param = 0x1E;
        *slot = param;
        slot = &actor->secondaryX;
        param = 0x32;
        *slot = param;
        slot = &actor->secondaryY;
        param = 0x1E;
        *slot = param;
        slot = &actor->secondaryZ;
        param = 0x1E;
        *slot = param;
        slot = &actor->alternateX;
        param = 0x1E;
        *slot = param;
        slot = &actor->alternateY;
        value = 0x1E;
        *slot = value;
        slot = &actor->alternateZ;
    }
    *slot = value;
    if (Script_SeekToKeyword(0x73)) {
        actor->cloneX = Script_GetValue();
        actor->cloneY = Script_GetValue();
        actor->cloneZ = Script_GetValue();
        actor->cloneSecondaryX = Script_GetValue();
        actor->cloneSecondaryY = Script_GetValue();
        value = Script_GetValue();
        slot = &actor->cloneSecondaryZ;
    } else {
        slot = &actor->cloneX;
        *slot = 0x1E;
        actor->cloneY = 0x1E;
        slot = &actor->cloneZ;
        *slot = 0x1E;
        slot = &actor->cloneSecondaryX;
        *slot = 0x1E;
        slot = &actor->cloneSecondaryY;
        value = 0x1E;
        *slot = value;
        slot = &actor->cloneSecondaryZ;
    }
    *slot = value;
    if (Script_SeekToKeyword(0x52)) {
        actor->rangeX = Script_GetValue();
        actor->rangeY = Script_GetValue();
        actor->rangeZ = Script_GetValue();
    } else {
        actor->rangeX = 0x1E;
        actor->rangeY = 5;
        actor->rangeZ = 0x1E;
    }
    if (Script_SeekToKeyword(0x47)) {
        actor->parameterEE6 = Script_GetValue();
    } else {
        actor->parameterEE6 = 0x14;
    }
    if (Script_SeekToKeyword(0x62)) {
        for (i = 0; i < 6; i++) {
            actor->orbSlots[i].position.x = Script_GetValue();
            actor->orbSlots[i].position.y = Script_GetValue();
            actor->orbSlots[i].position.z = Script_GetValue();
        }
    }
    if (Script_SeekToKeyword(0x44)) {
        animationGate = Script_GetValue();
    } else {
        animationGate = 0;
    }
    if (Script_SeekToKeyword(0x64)) {
        facing = (u8)(((Script_GetValue() << 29) + 0xA0000000u) >> 24);
    }
    if (Script_SeekToKeyword(0x58)) {
        actor->parameterED2 = Script_GetValue();
    } else {
        actor->parameterED2 = 0x3E8;
    }
    if (Script_SeekToKeyword(0x74)) {
        actor->parameterF0C = Script_GetValue();
    } else {
        actor->parameterF0C = 0;
    }
    RatatoskInit_SetTarget(self, gUnknown_02000580);
    SceneActors_Initialize(self, id, 4, &sizes, facing);
    self->savedFacing = facing;
    actor->displayResource = 0xFB81;
    SceneResource_InitializeHiddenSprite(actor);
    SceneActors_StartUnmirroredAnimation(self, actor->animationData, actor->animation, 7, 1, 0);

    self->specialMode = Script_GetKeywordValue(0x69, 0);
    if (self->specialMode == 0) {
        pdata = Path_GetPointData(Path_GetDefinition(0));
        for (i = 0; i <= 8; i++) {
            Path_GetPointPosition(&pv, pdata, i);
            actor->points[i] = pv;
            actor->points[i].y = 0x200;
        }
        pdata = Path_GetPointData(Path_GetDefinition(1));
        for (i = 0; i <= 8; i++) {
            Path_GetPointPosition(&pv, pdata, i);
            actor->orbSlots[i].position = pv;
            actor->orbSlots[i].position.y = 0x200;
        }
    }
    RatatoskInit_SetDamageHandler(self, sub_081F6428);
    RatatoskInit_SetStateHandler(self, Ratatosk_InitRequestedState);
    SceneActors_SetHealthLimits(self, hpA, hpA, hpB);

    if (!Script_SeekToKeyword(0x70)) {
        self->body.position = actor->points[0];
    }
    Ratatosk_InitHurtCollider(actor, hpB);
    Ratatosk_InitBodyCollider(actor);
    Ratatosk_InitFrontCollider(actor);
    Ratatosk_InitWideBodyCollider(actor, hpB);
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
    actor->blendEnabled = 0;
    actor->blendResource = 0x1F2;
    actor->blendMode = 0;
    actor->blendShift = 5;
    actor->delay = 0x3C;
    self->flags |= 0x10;

    if (self->specialMode == 0) {
        RatatoskInit_ConfigureAnimation(self, 0, 1);
    } else if (animationGate == 0) {
        RatatoskInit_ConfigureAnimation(self, 2, 1);
    } else {
        RatatoskInit_ConfigureAnimation(self, 2, 0);
    }
    actor->parameterED4 = 0;
    actor->hurtX = defaultHurtX;
    actor->hurtY = defaultHurtY;
    actor->scaleX = 0;
    actor->scaleZ = 0;
    actor->scaleStep = 0;
    actor->projectileCount = 3;
    actor->cloneCount = 3;
    actor->motionScale = 0x40;
    actor->savedState = 2;
    actor->motionScale = 0x80;
    gRandomIndex = (gRandomIndex + 1) & 0x3FF;
    k = Mod(((u16 *)RANDOM_TABLE_ADDR)[gRandomIndex], 9);
    actor->destination = *(k + actor->points);

    if (self->specialMode == 0) {
        struct RatatoskCamera *camera = &actor->camera;
        const void *targetPosition = RatatoskInit_GetTargetPosition(self);
        struct ContactPosition *origin = &actor->base.body.position;
        CameraFollow_Initialize(camera, targetPosition, origin, 1);
        RatatoskInit_SetCameraOrbit(camera, 140, 255);
        camera->duration = 30;
    }
    SceneActors_SetDamageCallback(self, actor->damageContext, Ratatosk_ApplyDamageReaction, 0);
    ScaledShadow_InitializeDefault(actor->shadow, &actor->anchor);
    if (self->specialMode)
        TerrainOverlay_QueueAddActor(&self->body);
    if (self->specialMode == 0) {
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
