#include "global.h"
void Script_ReadReference(void *);
void Script_WriteReferenceValue(void *, u32, u32);
struct G {
    u32 w0;
};
extern struct G *gSessionState;
void SaveData_WritePlayFramesToScript(void)
{
    u32 buf[2];
    Script_ReadReference(buf);
    Script_WriteReferenceValue(buf, 0, gSessionState->w0);
}
