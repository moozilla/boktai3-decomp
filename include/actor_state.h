#ifndef FOUNDRY_ACTOR_STATE_H
#define FOUNDRY_ACTOR_STATE_H
#include "scene_actor_components.h"
/* Tentative actor state prefix shared by the adjacent state callbacks. */
struct ActorAnimationPair {
    u32 first, second;
};
struct ActorState {
    u8 unknown0[0x48];
    struct SceneActorBody body;
    u32 stateTicks;
    s16 vitality;
    u8 unknown96[4];
    u8 checking;
    u8 unknown9B;
    u32 flags;
    u8 unknownA0[2];
    u8 stateReset, unknownA3;
    u32 pendingAction;
    u8 facing4, facing8;
    u8 state;
    u8 controlPhase;
    u8 active, phase, variant, pending, triggered;
    u8 changed, finished, special;
    u8 unknownB4[0x10];
    u32 timer;
    u32 threshold;
    u8 requestedDirection;
    u8 unknownCD[0x33];
    void *stateCallback;
    void *callback;
    u8 unknown108[4];
    void *damageTarget;
    struct ContactPosition *selectedPoint;
    s8 alternateDirection;
    u8 unknown115[0x173];
    u8 bounds[0x1C];
    struct ContactPosition destination;
    u8 animationData[0x20];
    u8 animationId;
    u8 unknown2CD[0x1C];
    u8 eventFlags;
    u8 unknown2EA[2];
    u8 eventScripts[0x28];
    void *palette;
    u8 unknown318[0x14];
    u8 effect[0xFC];
    u8 proximityVolume[0x40];
    s16 height;
    u16 lastDamage;
    u32 hitAttributes;
    u8 slowDown, direction, hitParameter, impactCount;
    u8 unknown474[3];
    u8 transitionBlocked;
    struct ActorAnimationPair selectedAnimation;
    struct ActorAnimationPair targetAnimation;
    u8 unknown488[0x58];
    u8 animationState, unknown4E1, animationPhase, animationIndex, animationDetail, mode;
    u16 decisionTimeout;
    u8 request, requestedVariant, requestMode, requestPhase;
    u8 unknown4EC[6];
    s16 floorHeight;
    s16 attackFlags;
    u8 unknown4F6[6];
    s16 hazardParameter;
    u8 unknown4FE[6];
    s8 paletteTimer;
    u8 unknown505, paletteMode, paletteShift;
    u8 unknown508[8];
    s8 randomOffset;
    u8 jitterTicks;
    u8 unknown512[0xE6];
    struct ContactPosition offsetPoint;
    u8 unknown600[0x600];
    u16 paletteData[16];
    struct ActorAnimationPair animation;
};
/* The owner retains this actor prefix at offset zero. */
struct ActorStateOwner {
    struct ActorState actor;
};
#endif
