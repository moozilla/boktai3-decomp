#include "global.h"
#include "scene/actor/scene_event_dispatch.h"
#include "bosses/vanargand/vanargand_pair_attack.h"
void SceneActors_ConfigureEffect(void *, s32, s32, s32);
u32 Trigger_ConsumePending(void *, u32);
void VanargandPair_DispatchTransitionCommands(struct VanargandPairAttack *boss)
{
    struct SceneEventReceiver *receiver = (struct SceneEventReceiver *)boss->trigger;
    s32 i = 0;
    if (i < receiver->count[gSceneEventDispatchState->phase]) {
        do {
            struct SceneEventDispatchState *globals = gSceneEventDispatchState;
            struct SceneEventCommand *command = receiver->pending[globals->phase][i];
            SceneMessages_SetPendingTrigger(receiver, command);
            switch (command->kind) {
                case 51:
                    Vanargand_StartAction(boss, 2, 0, 2);
                    break;
                case 52:
                    Vanargand_StartAction(boss, 2, 0, 3);
                    break;
                case 62:
                    Vanargand_StartAction(boss, 2, 0, 6);
                    break;
                case 54:
                    Vanargand_StartAction(boss, 2, 0, 7);
                    break;
                case 53:
                    Vanargand_StartAction(boss, 2, 0, 0);
                    break;
                case 55:
                    Vanargand_StartAction(boss, 2, 0, 10);
                    break;
                case 56:
                    Vanargand_StartAction(boss, 2, 0, 4);
                    break;
                case 57:
                    Vanargand_StartAction(boss, 2, 0, 5);
                    break;
                case 9:
                    SceneActors_ConfigureEffect(boss, command->value, 0xB4, 0xB4);
                    Trigger_ConsumePending(boss->trigger, 1);
                    break;
            }
            i++;
        } while (i < receiver->count[gSceneEventDispatchState->phase]);
    }
}
