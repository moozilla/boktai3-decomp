#include "global.h"
void Script_Call(u32, u32);
void Task_Kill(void *);

void sub_0810C774(u8 *p)
{
    u32 script = *(u32 *)(p + 0xf70);
    if (script) {
        Script_Call(script, 0);
        Task_Kill(p);
    }
}
