#include "scene_event_dispatch.h"
static inline s32 Count(struct SceneEventReceiver *receiver)
{
    u8 *phase = &gSceneEventDispatchState->phase;
    u8 *count = receiver->count;
    return count[*phase];
}
s32 SceneAudioEvent_DispatchPending(struct SceneEventDispatchTask *owner)
{
    struct SceneEventReceiver *receiver = &owner->receiver;
    s32 i = 0;
    while (i < Count(receiver)) {
        struct SceneEventDispatchState *state = gSceneEventDispatchState;
        struct SceneEventCommand *command;
        command = receiver->pending[state->phase][i];
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
        i++;
    }
    /* The task driver discards the original incidental return register. */
}
