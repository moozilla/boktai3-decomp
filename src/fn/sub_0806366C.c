#include "scene_event_dispatch.h"
#define SceneAudioEvent_StartTrackedSong sub_0806366C
void sub_0822AE7C(s32);
void Trigger_ConsumePending(void *, s32);
/* Preserve the signed full-word command argument observed at this call site. */
void SceneAudioEvent_StartTrackedSong(struct SceneEventDispatchTask *owner,
                                      struct SceneEventReceiver *receiver,
                                      struct SceneEventCommand *command)
{
    sub_0822AE7C(command->value);
    Trigger_ConsumePending(receiver, 1);
}
