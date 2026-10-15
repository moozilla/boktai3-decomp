#ifndef GUARD_STACKED_ACTOR_H
#define GUARD_STACKED_ACTOR_H
#include "game_state.h"
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
    u16 tileBase, animationPhase;
    struct StackedActorParticle particles[8];
    u8 lift, interactionEnabled;
    u16 interactionDelay;
    u32 pendingScriptId, completedScriptId, particleScriptId, particleScriptValue, acceptedScriptId;
    u16 unknown380, frame;
    void (*update)(struct StackedActor *);
};
extern u32 gUnk_03005308;
extern struct StackedActor *gUnk_02000118;
void sub_08052628(struct StackedActor *, void (*)(struct StackedActor *));
void sub_08052418(struct StackedActor *);
void sub_08052744(struct StackedActor *);
void sub_080527F0(struct StackedActor *);
void sub_0805284C(struct StackedActor *);
void sub_08052878(struct StackedActor *);
void sub_080528D0(struct StackedActor *);
void sub_08052A40(struct StackedActor *);
void sub_08052C28(struct StackedActor *);
void sub_08052CE4(struct StackedActor *);
void sub_08052D34(struct StackedActor *);
void sub_08052D60(struct StackedActor *);
void sub_08052EAC(struct StackedActor *);
void sub_08052F1C(struct StackedActor *);
void sub_08052F48(struct StackedActor *);
void sub_08052FE4(struct StackedActor *);
void sub_080521DC(struct StackedActorParticle *, struct StackedActorPosition *);
void sub_0805228C(struct StackedActorParticle *, struct StackedActorPosition *);
void sub_08052640(struct StackedActor *);
void sub_0805278C(struct StackedActor *);
void sub_08052938(struct StackedActor *);
void sub_08052E80(struct StackedActor *);
void sub_08052130(struct StackedActorParticle *);
void sub_0805321C(struct StackedActor *);
void sub_080530EC(struct StackedActor *);
s32 sub_08053178(struct StackedActor *);
s32 sub_080531BC(struct StackedActor *);
#endif
