#ifndef GUARD_OTENKO_ACTOR_H
#define GUARD_OTENKO_ACTOR_H

#include "effects/scene/scene_arc.h"
#include "engine/video/display_animation.h"
#include "scene/actor/scene_actor_components.h"
#include "scene/actor/scene_event_types.h"

struct TransitionEffect;
struct OtenkoEffectState {
    struct TransitionEffect *transition;
    u32 particleHandle;
    u8 unknown8;
    u8 bobCounter;
    s16 bobOffset;
    u8 projected, attached;
    s16 paletteFrame;
    u8 unknown10[2];
    u16 dashDuration;
    u8 unknown14[0xC];
};

struct OtenkoProjection {
    u8 unknown0[0x10];
    struct ScenePoolVec32 position;
    u8 unknown20[0x18];
    s32 height;
};

/* Otenko uses the scene actor allocation with extra effect and projection
 * state. Body motion, playback, and command storage use the scene types. */
struct OtenkoActor {
    u8 active, variant, stage, stageEntered, state, nextState, prevState, done;
    u8 commandEntered, animationEntered;
    u8 unknownA[2];
    u16 commandFlags, displayFlags;
    u32 stateFrame, timer;
    u8 animationPhase, facing4, facing8, targetAngle;
    u8 unknown1C[4];
    struct OtenkoEffectState effect;
    u8 unknown40[0x14];
    struct ContactPosition target;
    struct SceneActorBody body;
    u8 unknownA4[0x30];
    struct DisplayAnimation display;
    struct SceneEventReceiver receiver;
    u8 unknown190[0x3C];
    u16 workPalette[16];
    u8 unknown1EC[8];
    u16 extentX, extentZ;
    u32 rangeX, rangeZ, rangeXSquared, rangeZSquared;
    s32 speed, speedSquared;
    u8 unknown210[0xB];
    u8 controlled;
    u8 unknown21C[0xB];
    u8 savedState;
    u8 unknown228[0x14];
    struct ContactPosition savedPosition;
    u8 unknown244[0x88];
    void (*stateCallback)(void);
    void *callbacks[5];
    s32 (*animationCallback)(struct OtenkoActor *);
    u8 unknown2E8[8];
    struct OtenkoProjection projection;
    u8 unknown32C[0x18];
    struct SceneGroundEffect groundEffect;
};

typedef char OtenkoActorSizeCheck[sizeof(struct OtenkoActor) == 0x384 ? 1 : -1];

#endif
