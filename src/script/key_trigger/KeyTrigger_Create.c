#include "global.h"
#include "engine/engine.h"
#include "script/key_trigger.h"

void KeyTrigger_Update(void *);
void KeyTrigger_Destroy(void *);
s32 KeyTrigger_Init(void *);

void *KeyTrigger_Create(u16 arg)
{
    struct Task08197F68 *task = Task_Create(2, sizeof(*task));
    if (task) {
        task->channel = arg;
        Task_SetCallbacks(task, KeyTrigger_Update, KeyTrigger_Destroy);
        if (KeyTrigger_Init(task) < 0) {
            Task_Kill(task);
            return NULL;
        }
    }
    return task;
}
