#include "global.h"

struct S {
    u8 pad[0x18];
    u32 f18;
    u8 pad2[0x10];
    u32 f2c;
};
void Script_Call(u32, u32);
void Task_Kill(u32);

void sub_081C4C18(struct S *p)
{
    if (p->f2c)
        Script_Call(p->f2c, 0);
    Task_Kill(p->f18);
    Task_Kill((u32)p);
}
