#include "scene_event_dispatch.h"
#define SceneAudioEvent_StartChannel12 sub_080636DC
void sub_0822B0C8(s32);
void Trigger_ConsumePending(void *, s32);
/* Preserve the signed full-word command argument observed at this call site. */
void SceneAudioEvent_StartChannel12(struct SceneEventDispatchTask *owner,
                                    struct SceneEventReceiver *receiver,
                                    struct SceneEventCommand *command)
{
    sub_0822B0C8(command->value);
    Trigger_ConsumePending(receiver, 1);
}
