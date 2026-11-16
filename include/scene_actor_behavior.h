#ifndef GUARD_SCENE_ACTOR_BEHAVIOR_H
#define GUARD_SCENE_ACTOR_BEHAVIOR_H
#include "scene_actor_registry.h"
#include "room_actor.h"
/* Tentative view shared by the behavior helpers beginning at08078078. */
struct SceneActorBehaviorContactView {
    u8 unknown0[6];
    u16 flags;
    u8 unknown8[2];
    u16 kind;
    struct ContactPosition position;
    u8 unknown14[8];
    struct ContactPosition bounds;
    u8 unknown24[0x10];
    u32 parameter34, reactionFlags;
    u16 unknown3C, remaining, power;
    u8 direction, mode;
    u16 parameter44;
    u8 unknown46[6];
    void *contactCallback;
    s32 contactArgument;
};
struct SceneActorBehaviorDestination {
    u16 flags;
    u8 unknown2[10];
    struct ContactPosition position;
    u8 unknown14[8];
    struct ContactPosition destination;
};
#endif
