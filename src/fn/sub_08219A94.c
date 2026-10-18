#include "global.h"

void sub_0810BB0C(void);
void Wireless_UpdateController(void);
void sub_08219B24(void);
void MessageQueue_SwapBuffers(void);

s32 TaskSystem_UpdateRoot(void)
{
    sub_0810BB0C();
    Wireless_UpdateController();
    sub_08219B24();
    MessageQueue_SwapBuffers();
    return 0;
}
