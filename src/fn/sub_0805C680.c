#include "scene_panel_commands.h"
#define ScenePanel_DispatchCommands sub_0805C680
void ScenePanel_DispatchCommands(struct ScenePanel *owner)
{
    struct ScenePanelInteractionView *panel = (void *)owner;
    struct ScenePanelReceiver *receiver = &((struct ScenePanelCommandView *)owner)->receiver;
    s32 i = 0;
    if (i < receiver->count[gUnk_0200047C->bank]) {
        do {
            struct ScenePanelCommandGlobals *globals = gUnk_0200047C;
            struct ScenePanelCommand *command = receiver->commands[globals->bank][i];
            sub_08020D54(receiver, command);
            switch (command->opcode) {
                case 0:
                    panel->savedMode = command->mode;
                    Trigger_ConsumePending(receiver, 1);
                    break;
                case 1:
                    sub_08058150((void *)panel, sub_080581A0);
                    Trigger_ConsumePending(receiver, 1);
                    break;
            }
            i++;
        } while (i < receiver->count[gUnk_0200047C->bank]);
    }
}
