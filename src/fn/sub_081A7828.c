#include "global.h"
#include "scene_event_dispatch.h"
struct ScenePanelCommandState {
    u8 unknown0[0xAC];
    u8 mode, substate, transition, unknownAF, pending;
    u8 unknownB1[0xC4 - 0xB1];
    u32 counter;
    u8 unknownC8[0x118 - 0xC8];
    struct SceneEventReceiver receiver;
};
void SceneActors_ConfigureEffect(void *, s32, s32, s32);
u32 Trigger_ConsumePending(void *, u32);
static inline void ScenePanel_RequestTransition(struct ScenePanelCommandState *panel, u32 mode,
                                                u32 substate, u32 transition)
{
    panel->mode = mode;
    panel->substate = substate;
    panel->transition = transition;
    panel->counter = 0;
    panel->pending = 1;
}
void ScenePanel_DispatchTransitionCommands(struct ScenePanelCommandState *panel)
{
    struct SceneEventReceiver *receiver = &panel->receiver;
    s32 i = 0;
    if (i < receiver->count[gSceneEventDispatchState->phase]) {
        do {
            struct SceneEventDispatchState *globals = gSceneEventDispatchState;
            struct SceneEventCommand *command = receiver->pending[globals->phase][i];
            SceneMessages_SetPendingTrigger(receiver, command);
            switch (command->kind) {
                case 51:
                    ScenePanel_RequestTransition(panel, 2, 0, 2);
                    break;
                case 52:
                    ScenePanel_RequestTransition(panel, 2, 0, 3);
                    break;
                case 62:
                    ScenePanel_RequestTransition(panel, 2, 0, 6);
                    break;
                case 54:
                    ScenePanel_RequestTransition(panel, 2, 0, 7);
                    break;
                case 53:
                    ScenePanel_RequestTransition(panel, 2, 0, 0);
                    break;
                case 55:
                    ScenePanel_RequestTransition(panel, 2, 0, 10);
                    break;
                case 56:
                    ScenePanel_RequestTransition(panel, 2, 0, 4);
                    break;
                case 57:
                    ScenePanel_RequestTransition(panel, 2, 0, 5);
                    break;
                case 9:
                    SceneActors_ConfigureEffect(panel, command->value, 0xB4, 0xB4);
                    Trigger_ConsumePending(&panel->receiver, 1);
                    break;
            }
            i++;
        } while (i < receiver->count[gSceneEventDispatchState->phase]);
    }
}
