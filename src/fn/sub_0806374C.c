#include "scene_event_dispatch.h"
#define SceneAudioEvent_DispatchPending sub_0806374C
static inline s32 Count(struct SceneEventReceiver *receiver)
{
    u8 *phase = &gUnk_0200047C->phase;
    u8 *count = receiver->count;
    return count[*phase];
}
s32 SceneAudioEvent_DispatchPending(struct SceneEventDispatchTask *owner)
{
    struct SceneEventReceiver *receiver = &owner->receiver;
    s32 i = 0;
    while (i < Count(receiver)) {
        struct SceneEventDispatchState *state = gUnk_0200047C;
        struct SceneEventCommand *command;
        command = receiver->pending[state->phase][i];
        sub_08020D54(receiver, command);
        switch ((u32)command->kind) {
            case 0:
                sub_08063634(owner, receiver, command);
                break;
            case 1:
                sub_08063650(owner, receiver, command);
                break;
            case 2:
                sub_0806366C(owner, receiver, command);
                break;
            case 3:
                sub_08063688(owner, receiver, command);
                break;
            case 4:
                sub_080636A4(owner, receiver, command);
                break;
            case 5:
                sub_080636C0(owner, receiver, command);
                break;
            case 6:
                sub_080636DC(owner, receiver, command);
                break;
            case 7:
                sub_080636F8(owner, receiver, command);
                break;
            case 8:
                sub_08063714(owner, receiver, command);
                break;
            case 9:
                sub_08063730(owner, receiver, command);
                break;
        }
        i++;
    }
}
