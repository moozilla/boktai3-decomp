#include "bosses/sabata/sabata_attack_state.h"
#include "objects/pile_driver/pile_driver_commands.h"
void sub_0817B2DC(struct SabataAttackActor *p)
{
    s32 i;
    struct PileDriverReceiver *receiver;
    receiver = &p->receiver;
    i = 0;
    if (i < p->receiver.count[gSceneEventDispatchState->bank]) {
        do {
            struct PileDriverCommandGlobals *globals = gSceneEventDispatchState;
            s32 op;
            struct PileDriverCommand *command = receiver->commands[globals->bank][i];
            SceneMessages_SetPendingTrigger(receiver, command);
            op = command->opcode;
            switch (op) {
                case 1: {
                    s32 zero = 0;
                    u8 *action = &p->action;
                    u8 *subaction = &p->subaction;
                    u8 *choice = &p->choice;
                    u32 *timer = &p->actionTimer;
                    *action = zero;
                    *subaction = zero;
                    *choice = zero;
                    *timer = zero;
                    p->queued = op;
                    break;
                }

                case 5:
                    p->alternateDirection = (command->mode + 5) << 5;
                    p->action = 2;
                    p->subaction = 0;
                    p->choice = 1;
                    p->actionTimer = 0;
                    p->queued = 1;
                    break;

                case 27:
                    p->action = 2;
                    p->subaction = 0;
                    p->choice = 2;
                    p->actionTimer = 0;
                    p->queued = 1;
                    break;

                case 28: {
                    s32 mode = 3;
                    p->action = 2;
                    p->subaction = 0;
                    p->choice = mode;
                    p->actionTimer = 0;
                    p->queued = 1;
                    break;
                }
            }

            i++;
        } while (i < receiver->count[gSceneEventDispatchState->bank]);
    }
}
