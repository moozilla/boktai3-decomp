#include "global.h"
#include "scene_event_dispatch.h"

struct SceneObj3EF0 {
    u8 unknown0[0x60];
    struct SceneEventReceiver receiver;
};

u32 Trigger_ConsumePending(void *, u32);
void RegistryF3E4_StartAppearing(void *);
void RegistryF3E4_StartDisappearing(void *);

void sub_081C3EF0(struct SceneObj3EF0 *obj)
{
    struct SceneEventReceiver *receiver = &obj->receiver;
    s32 i = 0;
    if (i < receiver->count[gSceneEventDispatchState->phase]) {
        do {
            struct SceneEventDispatchState *globals = gSceneEventDispatchState;
            struct SceneEventCommand *command = receiver->pending[globals->phase][i];
            SceneMessages_SetPendingTrigger(receiver, command);
            switch (command->kind) {
                case 9:
                    RegistryF3E4_StartAppearing(obj);
                    Trigger_ConsumePending(receiver, 1);
                    break;
                case 10:
                    RegistryF3E4_StartDisappearing(obj);
                    Trigger_ConsumePending(receiver, 1);
                    break;
            }
            i++;
        } while (i < receiver->count[gSceneEventDispatchState->phase]);
    }
}
