#ifndef GUARD_STACKED_ACTOR_H
#define GUARD_STACKED_ACTOR_H
#include "system/game_state/game_state.h"
#include "engine/engine_globals.h"
/* Tentative identity: an actor with three vertically stacked display segments. */
struct WarpCirclePosition {
    s16 x, y, z, pad;
} __attribute__((aligned(4)));
struct WarpCircleSprite {
    u32 flags;
    u8 unknown4[2], rotation, renderMode;
    u8 scaleX, scaleY, unknownA[6];
    u16 frame;
    u8 unknown12[10];
    struct WarpCirclePosition position;
    u8 unknown24[8];
};
struct WarpCircleParticle {
    u32 flags;
    u8 unknown4[0xB], renderMode, priority, unknown11[7];
    struct WarpCirclePosition position;
    u8 unknown20[8];
    u8 enabled, life, unknown2A[2];
    s16 velocityX, velocityY, velocityZ;
    u16 unknown32;
};
struct WarpCircle {
    u8 task[0x18], trigger[0x34];
    u16 proximityState, baseFrame;
    struct WarpCircleSprite sprite;
    u8 graphics[0x1C];
    struct WarpCirclePosition world;
    struct WarpCircleSprite shadow;
    u8 shadowGraphics[0x1C];
    struct WarpCircleSprite segments[3];
    u8 segmentGraphics[3][0x1C];
    u16 height;
    u8 removeAfterUse, tetherAccepted;
    u16 paletteBase, animationPhase;
    struct WarpCircleParticle particles[8];
    u8 lift, interactionEnabled;
    u16 interactionDelay;
    u32 pendingScriptId, completedScriptId, particleScriptId, particleScriptValue, acceptedScriptId;
    u16 unknown380, frame;
    void (*update)(struct WarpCircle *);
};
/* 0x02000118 */
extern struct WarpCircle *gStackedActor;
void WarpCircle_SetUpdate(struct WarpCircle *, void (*)(struct WarpCircle *));
void WarpCircle_UpdateSegments(struct WarpCircle *);
void WarpCircle_HoldLift(struct WarpCircle *);
void WarpCircle_ExtendSegments(struct WarpCircle *);
void WarpCircle_WaitForParticles(struct WarpCircle *);
void WarpCircle_EmitFallingParticles(struct WarpCircle *);
void WarpCircle_RetractToIdle(struct WarpCircle *);
void WarpCircle_TickIdle(struct WarpCircle *);
void WarpCircle_BeginPlayerInteraction(struct WarpCircle *);
void WarpCircle_ExtendForScript(struct WarpCircle *);
void WarpCircle_WaitForScriptParticles(struct WarpCircle *);
void WarpCircle_EmitScriptParticles(struct WarpCircle *);
void WarpCircle_EmitRisingParticles(struct WarpCircle *);
void WarpCircle_WaitForRetraction(struct WarpCircle *);
void WarpCircle_FinishPlayerInteraction(struct WarpCircle *);
void WarpCircle_Disappear(struct WarpCircle *);
void WarpCircle_InitRisingParticle(struct WarpCircleParticle *, struct WarpCirclePosition *);
void WarpCircle_InitFallingParticle(struct WarpCircleParticle *, struct WarpCirclePosition *);
void WarpCircle_TickDisabled(struct WarpCircle *);
void WarpCircle_LowerLift(struct WarpCircle *);
void WarpCircle_Appear(struct WarpCircle *);
void WarpCircle_WaitForPlayerRelease(struct WarpCircle *);
void WarpCircle_TickParticle(struct WarpCircleParticle *);
void WarpCircle_InitParticles(struct WarpCircle *);
void WarpCircle_ConsumeTriggers(struct WarpCircle *);
s32 WarpCircle_Tick(struct WarpCircle *);
s32 WarpCircle_Destroy(struct WarpCircle *);
#endif
