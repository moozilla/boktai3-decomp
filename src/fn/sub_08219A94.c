#include "global.h"

void SessionMenu_InvokeCallback(void);
void Wireless_UpdateController(void);
void sub_08219B24(void);
void MessageQueue_SwapBuffers(void);

s32 TaskSystem_UpdateRoot(void)
{
    SessionMenu_InvokeCallback();
    Wireless_UpdateController();
    sub_08219B24();
    MessageQueue_SwapBuffers();
    return 0;
}
