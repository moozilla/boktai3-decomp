#ifndef GUARD_SCENE_ACTOR_ANIMATION_H
#define GUARD_SCENE_ACTOR_ANIMATION_H
#include "scene_actor_profile.h"
/* Animation-phase actor and drawable views; selection names are provisional. */
struct SceneActorAnimationView {
    void *object;
    void *drawable;
    u8 drawableKind, unknown9[0x50];
    u8 direction;
    u8 unknown5A[0x8F];
    u8 facingAngle;
    u8 unknownEA[6];
    u32 phaseTimer;
    u8 unknownF4[0x1B4];
    u8 phase, subphase, nextPhase, unknown2AB;
    u32 phaseCounter;
    u8 unknown2B0, phaseReady;
    u8 directionalEntryFlag;
    u8 restart;
    u8 unknown2B4[3];
    u8 selection;
    u8 unknown2B8[0xC];
    u32 profileMode;
    u8 unknown2C8[0x40];
    u32 profileId;
    u8 unknown30C[0xC1];
    u8 nextDirection;
    u8 unknown3CE[2];
    struct SceneActorAnimationLink *link;
};
struct SceneActorAnimationLink {
    u8 unknown0[0x6CC];
    u32 phaseTimer;
    u8 unknown6D0[4];
    u16 flags;
    u8 unknown6D6[0x12];
    u16 phaseDuration;
};
struct SceneActorAnimationSequenceDrawable {
    u8 unknown0[0x48];
    u8 sequence[0x10];
    const void *resource;
};
struct SceneActorAnimationTile {
    u8 unknown0[8];
    u32 flags;
};
struct SceneActorAnimationTileDrawable {
    u8 unknown0[0x20];
    struct SceneActorAnimationTile tile;
};
void SceneActorAnimation_SelectDirectionalEntry(struct SceneActorAnimationView *, s32);
void SceneActorAnimation_SelectDirectionalEntryFlags(struct SceneActorAnimationView *, s32, u32);
void sub_080762A8(struct SceneActorAnimationView *, s32, u32);
void SceneActorAnimation_SelectEntryFlags(struct SceneActorAnimationView *, s32, u32);
void SceneActorAnimation_SelectDirectionFlagsVariant(struct SceneActorAnimationView *, s32, u32);
void SceneActorAnimation_SelectExplicitVariant(struct SceneActorAnimationView *, s32, u32, u32);
void SceneActorAnimation_SelectDirectionalFlags(struct SceneActorAnimationView *, s32);
void SceneActorAnimation_SelectProfileEntry(struct SceneActorAnimationView *, s32);
#endif
