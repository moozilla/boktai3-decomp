#include "scene_event_dispatch.h"
#define SceneAudioEvent_FadeOutChannel10 sub_080636C0
void sub_0822B058(s32);
void Trigger_ConsumePending(void *, s32);
/* Preserve the signed full-word command argument observed at this call site. */
void SceneAudioEvent_FadeOutChannel10(struct SceneEventDispatchTask *owner,
                                      struct SceneEventReceiver *receiver,
                                      struct SceneEventCommand *command)
{
    sub_0822B058(command->value);
    Trigger_ConsumePending(receiver, 1);
}
