#include "global.h"
void Script_Call(u32, u32);
void Task_Kill(void *);
void sub_08110164(u8 *task)
{
    u16 *script = (u16 *)(task + 0x1028);
    if (*script) {
        Script_Call(*script, 0);
        Task_Kill(task);
    }
}
