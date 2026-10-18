#include "global.h"

extern u32 gUnk_02000484;
void Wireless_UpdateControllerTask(void);
void Wireless_UpdateController(void)
{
    if (gUnk_02000484)
        Wireless_UpdateControllerTask();
}
