#include "global.h"
#include "scene_panel.h"
#include "scene_panel_commands.h"
struct Registry1909 {
    u8 unknown0[0x98];
    struct ScenePanelReceiver receiver;
};
void Registry1909_StartFalling(struct Registry1909 *);
void sub_081C5264(struct Registry1909 *p)
{
    struct ScenePanelReceiver *receiver = &p->receiver;
    s32 i = 0;
    if (i < p->receiver.count[gSceneEventDispatchState->bank]) {
        do {
            struct ScenePanelCommandGlobals *globals = gSceneEventDispatchState;
            struct ScenePanelCommand *command = receiver->commands[globals->bank][i];
            SceneMessages_SetPendingTrigger(receiver, command);
            if (command->opcode == 8) {
                Registry1909_StartFalling(p);
                Trigger_ConsumePending(&p->receiver, 1);
            }
            i++;
        } while (i < receiver->count[gSceneEventDispatchState->bank]);
    }
}
