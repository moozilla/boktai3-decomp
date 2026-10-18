#include "global.h"
extern u32 gUnk_020003C8;
void Task_Kill(u32);
void Actor_RequestControllerRemoval(void)
{
    Task_Kill(gUnk_020003C8);
}
