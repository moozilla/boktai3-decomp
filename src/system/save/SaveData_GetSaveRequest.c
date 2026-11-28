#include "global.h"
#include "link/session/session_save.h"

u32 SaveData_GetSaveRequest(void)
{
    return ((struct SessionSave *)gSessionState)->saveRequested;
}
