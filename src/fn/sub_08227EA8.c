#include "global.h"
#include "runtime.h"
void Script_ReadReference(void *);
void Script_WriteReferenceValue(void *, u32, u32);
struct G {
    u32 w0;
};
void SaveData_WritePlayFramesToScript(void)
{
    u32 buf[2];
    Script_ReadReference(buf);
    Script_WriteReferenceValue(buf, 0, ((struct G *)gSessionState)->w0);
}
