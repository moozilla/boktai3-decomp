#include "global.h"
#include "engine.h"
#define KeyTrigger_Create sub_08198008

void sub_08197EC4(void *);
void sub_08197F64(void *);
s32 sub_08197F68(void *);

void *sub_08198008(u16 arg)
{
    u8 *task = Task_Create(2, 0x50);
    if (task) {
        *(u32 *)(task + 0x18) = arg;
        Task_SetCallbacks(task, sub_08197EC4, sub_08197F64);
        if (sub_08197F68(task) < 0) {
            Task_Kill(task);
            return NULL;
        }
    }
    return task;
}
