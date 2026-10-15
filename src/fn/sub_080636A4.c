#include "scene_event_dispatch.h"
#define SceneAudioEvent_PauseChannel10 sub_080636A4
void sub_0822AFE8(s32);
void Trigger_ConsumePending(void *, s32);
/* Preserve the signed full-word command argument observed at this call site. */
void SceneAudioEvent_PauseChannel10(struct SceneEventDispatchTask *owner,
                                    struct SceneEventReceiver *receiver,
                                    struct SceneEventCommand *command)
{
    sub_0822AFE8(command->value);
    Trigger_ConsumePending(receiver, 1);
}
