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
struct SceneActorRegistryActor *sub_08070EBC(u32);
#endif
