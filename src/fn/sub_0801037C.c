#include "group_objects.h"
static inline int count(struct GroupQueue *q)
{
    u8 *bank = &gUnk_0200047C->bank;
    return q->count[*bank];
}
int BoardPieces_DispatchCommands(struct GroupObjects *manager)
{
    struct GroupQueue *queue = &manager->queue;
    int i;
    for (i = 0; i < count(queue); i++) {
        struct GroupGlobals *g = gUnk_0200047C;
        {
            struct GroupCommand *cmd = queue->commands[g->bank][i];
            sub_08020D54(queue, cmd);
            switch (cmd->type) {
                case 0:
                    sub_0800FC28(manager, queue, cmd);
                    Trigger_ConsumePending(&manager->queue, 1);
                    break;
                case 1:
                    sub_0800FE98(manager, queue, cmd);
                    Trigger_ConsumePending(&manager->queue, 1);
                    break;
                case 2:
                    sub_08010298(manager, queue, cmd);
                    Trigger_ConsumePending(&manager->queue, 1);
                    break;
                case 11:
                    sub_0801010C(manager, queue, cmd);
                    Trigger_ConsumePending(&manager->queue, 1);
                    break;
            }
        }
    }
    return 0;
}
