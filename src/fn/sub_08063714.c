#include "scene_event_dispatch.h"
void Sound_PauseChannel12(s32);
void Trigger_ConsumePending(void *, s32);
/* Preserve the signed full-word command argument observed at this call site. */
void SceneAudioEvent_PauseChannel12(struct SceneEventDispatchTask *owner,
                                    struct SceneEventReceiver *receiver,
                                    struct SceneEventCommand *command)
{
    Sound_PauseChannel12(command->value);
    Trigger_ConsumePending(receiver, 1);
}
