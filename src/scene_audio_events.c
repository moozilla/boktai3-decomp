#include "scene_audio_events.h"

void SceneAudioEvent_PlaySong(struct SceneEventDispatchTask *owner,
                              struct SceneEventReceiver *receiver,
                              struct SceneEventCommand *command)
{
    Sound_PlaySong(command->value);
    Trigger_ConsumePending(receiver, 1);
}

void SceneAudioEvent_StartSongDirect(struct SceneEventDispatchTask *owner,
                                     struct SceneEventReceiver *receiver,
                                     struct SceneEventCommand *command)
{
    Sound_StartSongDirect(command->value);
    Trigger_ConsumePending(receiver, 1);
}

void SceneAudioEvent_StartTrackedSong(struct SceneEventDispatchTask *owner,
                                      struct SceneEventReceiver *receiver,
                                      struct SceneEventCommand *command)
{
    Sound_PlayTrackedSong(command->value);
    Trigger_ConsumePending(receiver, 1);
}

void SceneAudioEvent_FadeInChannel10(struct SceneEventDispatchTask *owner,
                                     struct SceneEventReceiver *receiver,
                                     struct SceneEventCommand *command)
{
    Sound_FadeInChannel10(command->value);
    Trigger_ConsumePending(receiver, 1);
}

void SceneAudioEvent_PauseChannel10(struct SceneEventDispatchTask *owner,
                                    struct SceneEventReceiver *receiver,
                                    struct SceneEventCommand *command)
{
    Sound_PauseChannel10(command->value);
    Trigger_ConsumePending(receiver, 1);
}

void SceneAudioEvent_FadeOutChannel10(struct SceneEventDispatchTask *owner,
                                      struct SceneEventReceiver *receiver,
                                      struct SceneEventCommand *command)
{
    Sound_FadeOutChannel10(command->value);
    Trigger_ConsumePending(receiver, 1);
}

void SceneAudioEvent_StartChannel12(struct SceneEventDispatchTask *owner,
                                    struct SceneEventReceiver *receiver,
                                    struct SceneEventCommand *command)
{
    Sound_PlayChannel12Song(command->value);
    Trigger_ConsumePending(receiver, 1);
}

void SceneAudioEvent_FadeInChannel12(struct SceneEventDispatchTask *owner,
                                     struct SceneEventReceiver *receiver,
                                     struct SceneEventCommand *command)
{
    Sound_FadeInChannel12(command->value);
    Trigger_ConsumePending(receiver, 1);
}

void SceneAudioEvent_PauseChannel12(struct SceneEventDispatchTask *owner,
                                    struct SceneEventReceiver *receiver,
                                    struct SceneEventCommand *command)
{
    Sound_PauseChannel12(command->value);
    Trigger_ConsumePending(receiver, 1);
}

void SceneAudioEvent_FadeOutChannel12(struct SceneEventDispatchTask *owner,
                                      struct SceneEventReceiver *receiver,
                                      struct SceneEventCommand *command)
{
    Sound_FadeOutChannel12(command->value);
    Trigger_ConsumePending(receiver, 1);
}

static inline s32 SceneAudioEvent_GetPendingCount(struct SceneEventReceiver *receiver)
{
    u8 *phase = &gSceneEventDispatchState->phase;
    u8 *count = receiver->count;
    return count[*phase];
}

s32 SceneAudioEvent_DispatchPending(struct SceneEventDispatchTask *owner)
{
    struct SceneEventReceiver *receiver = &owner->messages.queue;
    s32 index = 0;
    while (index < SceneAudioEvent_GetPendingCount(receiver)) {
        struct SceneEventDispatchState *state = gSceneEventDispatchState;
        struct SceneEventCommand *command;
        command = receiver->pending[state->phase][index];
        SceneMessages_SetPendingTrigger(receiver, command);
        switch ((u32)command->kind) {
            case 0:
                SceneAudioEvent_PlaySong(owner, receiver, command);
                break;
            case 1:
                SceneAudioEvent_StartSongDirect(owner, receiver, command);
                break;
            case 2:
                SceneAudioEvent_StartTrackedSong(owner, receiver, command);
                break;
            case 3:
                SceneAudioEvent_FadeInChannel10(owner, receiver, command);
                break;
            case 4:
                SceneAudioEvent_PauseChannel10(owner, receiver, command);
                break;
            case 5:
                SceneAudioEvent_FadeOutChannel10(owner, receiver, command);
                break;
            case 6:
                SceneAudioEvent_StartChannel12(owner, receiver, command);
                break;
            case 7:
                SceneAudioEvent_FadeInChannel12(owner, receiver, command);
                break;
            case 8:
                SceneAudioEvent_PauseChannel12(owner, receiver, command);
                break;
            case 9:
                SceneAudioEvent_FadeOutChannel12(owner, receiver, command);
                break;
        }
        index++;
    }
    /* The task driver discards the original incidental return register. */
}

s32 SceneAudioEvent_Tick(struct SceneEventDispatchTask *owner)
{
    SceneAudioEvent_DispatchPending(owner);
    return 0;
}

s32 SceneAudioEvent_Destroy(void)
{
    return 0;
}

s32 SceneAudioEvent_Register(struct SceneEventDispatchTask *owner, s32 receiverId)
{
    SceneMessages_RegisterReceiver(&owner->messages, receiverId, 8);
    return 0;
}

struct SceneEventDispatchTask *SceneAudioEvent_Create(s32 receiverId)
{
    struct SceneEventDispatchTask *owner = Task_Create(11, sizeof(struct SceneEventDispatchTask));
    if (owner) {
        Task_SetCallbacks(owner, (void (*)(void *))SceneAudioEvent_Tick,
                          (void (*)(void *))SceneAudioEvent_Destroy);
        if (SceneAudioEvent_Register(owner, receiverId) < 0) {
            Task_Kill(owner);
            return 0;
        }
    }
    return owner;
}
