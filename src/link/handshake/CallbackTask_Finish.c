#include "global.h"
#include "engine/engine.h"
#include "link/handshake/link_callback_task.h"

void Serial_StopCommunication(void);

u8 CallbackTask_Finish(struct LinkCallbackTask *task)
{
    if (task->completed) {
        Serial_StopCommunication();
        task->acknowledged = 1;
        if (task->result) {
            if (task->success)
                task->success(task->context, task->arg1, task->arg2, task->arg3);
        } else {
            if (task->failure)
                task->failure(task->context, task->arg1, task->arg2, task->arg3);
        }
        Task_Kill(task);
        return TRUE;
    }
    return FALSE;
}
