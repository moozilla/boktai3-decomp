#include "global.h"
#include "scene_event_dispatch.h"

struct SceneObj4950 {
    u8 unknown0[0x74];
    struct SceneEventReceiver receiver;
};

void ScenePanelSubject_SlideOpen(void *);

void sub_081C4950(struct SceneObj4950 *obj)
{
    struct SceneEventReceiver *receiver = &obj->receiver;
    s32 i = 0;
    if (i < receiver->count[gSceneEventDispatchState->phase]) {
        do {
            struct SceneEventDispatchState *globals = gSceneEventDispatchState;
            struct SceneEventCommand *command = receiver->pending[globals->phase][i];
            SceneMessages_SetPendingTrigger(receiver, command);
            if (command->kind == 7)
                ScenePanelSubject_SlideOpen(obj);
            i++;
        } while (i < receiver->count[gSceneEventDispatchState->phase]);
    }
}
