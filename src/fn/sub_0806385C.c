#include "scene_event_dispatch.h"
s32 SceneAudioEvent_DispatchPending(struct SceneEventDispatchTask *);
s32 SceneAudioEvent_Tick(struct SceneEventDispatchTask *owner)
{
    SceneAudioEvent_DispatchPending(owner);
    return 0;
}
