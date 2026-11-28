#include "global.h"
#include "link/session/session_save.h"
u32 SaveData_IsHeaderValid(void)
{
    if (((struct SessionSave *)gSessionState)->magic == 0x369F)
        return 1;
    return 0;
}
