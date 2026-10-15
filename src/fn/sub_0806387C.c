#include "scene_event_dispatch.h"
#include "engine.h"
#define SceneAudioEvent_Create sub_0806387C
s32 sub_0806386C(struct SceneEventDispatchTask *, s32);
s32 sub_0806385C(struct SceneEventDispatchTask *);
s32 sub_08063868(void);
struct SceneEventDispatchTask *SceneAudioEvent_Create(s32 receiverId)
{
    struct SceneEventDispatchTask *owner = Task_Create(11, 0x50);
    if (owner) {
        Task_SetCallbacks(owner, (void (*)(void *))sub_0806385C, (void (*)(void *))sub_08063868);
        if (sub_0806386C(owner, receiverId) < 0) {
            Task_Kill(owner);
            return 0;
        }
    }
    return owner;
}
