#ifndef GUARD_SCENE_CHARACTER_H
#define GUARD_SCENE_CHARACTER_H

#include "engine/video/display_animation.h"
#include "scene/actor/scene_actor_components.h"
#include "scene/actor/scene_event_types.h"

struct SceneCharacterEffects {
    u32 particleHandle;
    u16 orbId;
    s16 tag;
    u8 unknown8[8];
    s16 fadeMode, fadeCounter;
    u32 bankB, bankA;
};

/* The story-script view of the actor allocated by Actor_Allocate. Some
 * character kinds use the navigation scratch area for their working palette. */
struct SceneNpc {
    u8 active, kind, stage, stageEntered, state, nextState, prevState, done;
    u8 commandEntered, animationEntered;
    u8 unknownA[2];
    u16 commandFlags, displayFlags;
    u32 stateFrame, timer;
    u8 animationPhase, facing4, facing8, targetAngle;
    u16 defaultPaletteBank, paletteBank;
    struct SceneCharacterEffects effects;
    u8 unknown3C[0x54 - 0x3C];
    struct ContactPosition target;
    struct SceneActorBody body;
    u8 unknownA4[0xD4 - 0xA4];
    struct DisplayAnimation display;
    struct SceneEventReceiver receiver;
    u8 unknown190[0x1CC - 0x190];
    u16 workPalette[16];
    u8 unknown1EC[7];
    s8 routeId;
    u8 unknown1F4[0x208 - 0x1F4];
    s32 speed, speedSquared;
    u8 unknown210[0x21B - 0x210];
    u8 controlled;
    u8 unknown21C[0x2CC - 0x21C];
    void (*stateCallback)(void);
    void *callbacks[6];
};

typedef char SceneNpcScriptViewSizeCheck[sizeof(struct SceneNpc) == 0x2E8 ? 1 : -1];

#endif
