#include "global.h"
#include "engine.h"

void KeyTrigger_Update(void *);
void KeyTrigger_Destroy(void *);
s32 KeyTrigger_Init(void *);

void *KeyTrigger_Create(u16 arg)
{
    u8 *task = Task_Create(2, 0x50);
    if (task) {
        *(u32 *)(task + 0x18) = arg;
        Task_SetCallbacks(task, KeyTrigger_Update, KeyTrigger_Destroy);
        if (KeyTrigger_Init(task) < 0) {
            Task_Kill(task);
            return NULL;
        }
    }
    return task;
}
