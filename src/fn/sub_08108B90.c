#include "global.h"
/* Provisional names and field interpretations; confidence low. */
#define CallbackTask_Finish sub_08108B90
struct CallbackTask {
    u8 pad0[0x20];
    s32 arg2;
    s32 arg1;
    u32 result;
    void *pending;
    u32 acknowledged;
    u8 pad34[0x6F - 0x34];
    u8 arg3;
    u32 flags;
    void (*success)(void *, s32, s32, u32);
    void (*failure)(void *, s32, s32, u32);
    u32 unk7C;
    void *context;
};
void Serial_StopCommunication(void);
void Task_Kill(void *);
u8 sub_08108B90(struct CallbackTask *task)
{
    if (task->pending) {
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
        return 1;
    }
    return 0;
}
