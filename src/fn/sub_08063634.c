#include "scene_event_dispatch.h"
#define SceneAudioEvent_PlaySong sub_08063634
void Sound_PlaySong(s32);
void Trigger_ConsumePending(void *, s32);
/* Preserve the signed full-word command argument observed at this call site. */
void SceneAudioEvent_PlaySong(struct SceneEventDispatchTask *owner,
                              struct SceneEventReceiver *receiver,
                              struct SceneEventCommand *command)
{
    Sound_PlaySong(command->value);
    Trigger_ConsumePending(receiver, 1);
}
