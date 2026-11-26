#ifndef GUARD_SCENE_VOLUME_H
#define GUARD_SCENE_VOLUME_H

#include "engine/shared/engine.h"
#include "actors/task_4_e01/task4e01_projectile.h"
#include "player/player.h"
#include "engine/shared/contact_collider.h"
#include "menus/scene_panel_presentation/scene_panel.h"

/* Shared attack, pursuit, impact, and landing state. Behavior names are tentative. */
struct SceneVolumeActor {
    u8 pad0[0x51];
    u8 direction;
    u8 pad52[2];
    s16 position[4];
    s16 velocity[4];
    u8 pad64[0x38];
    u32 flags;
    u8 padA0[2];
    u8 phase;
    u8 padA3;
    u32 timer;
    u8 direction4, direction8;
    u8 state, padAB, action, subaction, choice, pending, queued, restart, finished, special;
    u8 padB4[0x10];
    u32 actionTimer;
    u8 padC8[4];
    u8 requestedDirection;
    u8 padCD[0x37];
    void *callback;
    u8 pad108[4];
    struct Player *target;
    void *positionSource;
    s8 alternateDirection;
    u8 pad115[3];
    struct ScenePanelReceiver receiver;
    u8 pad140[0x38];
    struct ContactCollider collider;
    u8 pad1CC[0xBC];
    u8 bounds[0x1C];
    s16 destination[4];
    u8 animationState[0x20];
    u8 animation;
    u8 pad2CD[0x47];
    void *sequence;
    u8 pad318[0x14];
    u8 effect[0x138];
    u32 effectFlags;
    s16 height, activityTimer;
    u8 pad46C[4];
    u8 slowDown, impactDirection, speed, impactCount;
    u8 pad474[0x14];
    struct Task4E01Params projectileParameters;
    u8 movementMode;
    u8 pad4E1;
    u8 reached, pathId, pointId;
    u8 pad4E5[3];
    u8 transition, previousChoice, decisionMode, decisionIndex;
    u8 pad4EC[4];
    u16 activity;
    u8 pad4F2[2];
    s16 attackFlags;
    u8 pad4F6[2];
    s16 projectilePower;
    u8 pad4FA[2];
    s16 parameter;
    u8 pad4FE[6];
    s8 sequenceTimer;
    u8 pad505, sequenceMode, sequenceShift;
    u32 sequenceLength;
    u8 pad50C[0xF4];
    u8 sequenceBuffer[0x600];
    u16 palette[16];
    u8 pathPosition[8];
    u8 padC28[0x38];
    u32 scripts[1]; /* indexed by decisionMode; extends past the shared layout */
};

static inline void SceneVolume_SetFlags(struct SceneVolumeActor *actor, u32 flags)
{
    actor->flags |= flags;
}

/* Takes a queued transition request: clears it and the pending trigger, 1 if there was one. */
static inline u8 SceneVolume_TakeQueued(struct SceneVolumeActor *actor)
{
    if (actor->queued) {
        actor->queued = 0;
        actor->pending = 0;
        return 1;
    }
    return 0;
}

void ActorEffect_InitializeOffset(struct SceneVolumeActor *);
void ActorEffect_UpdateTimedSequence(struct SceneVolumeActor *);
void SceneVolume_UpdateRepeatedShot(struct SceneVolumeActor *);
void SceneVolume_UpdatePursuitSpin(struct SceneVolumeActor *, s32);
void SceneVolume_UpdateImpactSlide(struct SceneVolumeActor *);
void SceneVolume_UpdateKnockbackLanding(struct SceneVolumeActor *, s32);
#endif
