#ifndef GUARD_STACKED_ACTOR_H
#define GUARD_STACKED_ACTOR_H
#include "game_state.h"
#include "engine_globals.h"
/* Tentative identity: an actor with three vertically stacked display segments. */
struct StackedActorPosition {
    s16 x, y, z, pad;
} __attribute__((aligned(4)));
struct StackedActorSprite {
    u32 flags;
    u8 unknown4[2], rotation, renderMode;
    u8 scaleX, scaleY, unknownA[6];
    u16 frame;
    u8 unknown12[10];
    struct StackedActorPosition position;
    u8 unknown24[8];
};
struct StackedActorParticle {
    u32 flags;
    u8 unknown4[0xB], renderMode, priority, unknown11[7];
    struct StackedActorPosition position;
    u8 unknown20[8];
    u8 enabled, life, unknown2A[2];
    s16 velocityX, velocityY, velocityZ;
    u16 unknown32;
};
struct StackedActor {
    u8 task[0x18], trigger[0x34];
    u16 proximityState, baseFrame;
    struct StackedActorSprite sprite;
    u8 graphics[0x1C];
    struct StackedActorPosition world;
    struct StackedActorSprite shadow;
    u8 shadowGraphics[0x1C];
    struct StackedActorSprite segments[3];
    u8 segmentGraphics[3][0x1C];
    u16 height;
    u8 removeAfterUse, tetherAccepted;
    u16 paletteBase, animationPhase;
    struct StackedActorParticle particles[8];
    u8 lift, interactionEnabled;
    u16 interactionDelay;
    u32 pendingScriptId, completedScriptId, particleScriptId, particleScriptValue, acceptedScriptId;
    u16 unknown380, frame;
    void (*update)(struct StackedActor *);
};
/* 0x02000118 */
extern struct StackedActor *gStackedActor;
void StackedActor_SetUpdate(struct StackedActor *, void (*)(struct StackedActor *));
void StackedActor_UpdateSegments(struct StackedActor *);
void StackedActor_HoldLift(struct StackedActor *);
void StackedActor_ExtendSegments(struct StackedActor *);
void StackedActor_WaitForParticles(struct StackedActor *);
void StackedActor_EmitFallingParticles(struct StackedActor *);
void StackedActor_RetractToIdle(struct StackedActor *);
void StackedActor_TickIdle(struct StackedActor *);
void StackedActor_BeginPlayerInteraction(struct StackedActor *);
void StackedActor_ExtendForScript(struct StackedActor *);
void StackedActor_WaitForScriptParticles(struct StackedActor *);
void StackedActor_EmitScriptParticles(struct StackedActor *);
void StackedActor_EmitRisingParticles(struct StackedActor *);
void StackedActor_WaitForRetraction(struct StackedActor *);
void StackedActor_FinishPlayerInteraction(struct StackedActor *);
void StackedActor_Disappear(struct StackedActor *);
void StackedActor_InitRisingParticle(struct StackedActorParticle *, struct StackedActorPosition *);
void StackedActor_InitFallingParticle(struct StackedActorParticle *, struct StackedActorPosition *);
void StackedActor_TickDisabled(struct StackedActor *);
void StackedActor_LowerLift(struct StackedActor *);
void StackedActor_Appear(struct StackedActor *);
void StackedActor_WaitForPlayerRelease(struct StackedActor *);
void StackedActor_TickParticle(struct StackedActorParticle *);
void StackedActor_InitParticles(struct StackedActor *);
void StackedActor_ConsumeTriggers(struct StackedActor *);
s32 StackedActor_Tick(struct StackedActor *);
s32 StackedActor_Destroy(struct StackedActor *);
#endif
