#ifndef GUARD_SCENE_ACTOR_SCRIPT_H
#define GUARD_SCENE_ACTOR_SCRIPT_H
#include "scene_actor_registry.h"
/* Tentative view of the script-controlled bytes in the actor's context tail. */
struct SceneActorScriptControlView {
    u8 unknown0[0x84];
    u8 parameter0;
    u8 unknown85;
    u8 parameter1;
};
struct SceneActorScriptPositionView {
    u8 unknown0[0x5C];
    struct ContactPosition position;
    u8 unknown64[0x25A];
    s16 groundHeight;
    u8 unknown2C0[0x2A];
    s16 heightOffset;
    u8 unknown2EC[8];
    u16 cellIndex;
};
struct SceneActorRegistryActor *sub_08070EBC(u32);
u8 *sub_0821D64C(u32, u32);
#endif
