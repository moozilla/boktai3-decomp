#include "scene_event_dispatch.h"
void sub_0822AF78(s32);
void Trigger_ConsumePending(void *, s32);
/* Preserve the signed full-word command argument observed at this call site. */
void SceneAudioEvent_FadeInChannel10(struct SceneEventDispatchTask *owner,
                                     struct SceneEventReceiver *receiver,
                                     struct SceneEventCommand *command)
{
    sub_0822AF78(command->value);
    Trigger_ConsumePending(receiver, 1);
}
