#include "scene_event_dispatch.h"
void SceneMessages_RegisterReceiver(void *, s32, s32);
s32 SceneAudioEvent_Register(struct SceneEventDispatchTask *owner, s32 receiverId)
{
    SceneMessages_RegisterReceiver(&owner->receiver, receiverId, 8);
    return 0;
}
