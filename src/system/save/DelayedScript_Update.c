#include "global.h"
#include "engine/engine.h"
#include "engine/message_queue.h"
#include "system/save/delayed_script.h"
s32 Script_RunBlock(u32, void *);
void DelayedScript_Update(struct DelayedScript *task)
{
    s32 count = MessageQueue_GetMessagesById(task->owner, &task->messages);
    s32 remaining = count;
    struct QueueMessage *message = task->messages;
    remaining--;
    if (count > 0) {
        do {
            if (*(u16 *)message->values == 0xFFFF)
                goto kill;
            message++;
        } while (remaining-- > 0);
    }
    {
        s32 timer = --task->timer;
        s32 result;
        if (timer > 0 && !(task->flags & DELAYED_SCRIPT_REPEAT))
            return;
        if (task->flags & DELAYED_SCRIPT_CALL)
            result = Script_Call(task->script, &task->args);
        else
            result = Script_RunBlock(task->script, &task->args);
        if (result == 0) {
            if ((task->flags & DELAYED_SCRIPT_REPEAT) && task->timer > 0)
                return;
        kill:
            Task_Kill(task);
        } else
            task->timer = result;
    }
}
