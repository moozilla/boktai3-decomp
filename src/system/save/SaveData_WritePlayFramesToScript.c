#include "global.h"
#include "link/session/session_save.h"
void Script_ReadReference(void *);
void Script_WriteReferenceValue(void *, u32, u32);
void SaveData_WritePlayFramesToScript(void)
{
    u32 buf[2];
    Script_ReadReference(buf);
    Script_WriteReferenceValue(buf, 0, ((struct SessionSave *)gSessionState)->unknown0);
}
