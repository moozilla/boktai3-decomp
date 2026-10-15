#include "scene_event_dispatch.h"
#include "engine.h"
s32 SceneAudioEvent_Register(struct SceneEventDispatchTask *, s32);
s32 SceneAudioEvent_Tick(struct SceneEventDispatchTask *);
s32 SceneAudioEvent_Destroy(void);
struct SceneEventDispatchTask *SceneAudioEvent_Create(s32 receiverId)
{
    struct SceneEventDispatchTask *owner = Task_Create(11, 0x50);
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
