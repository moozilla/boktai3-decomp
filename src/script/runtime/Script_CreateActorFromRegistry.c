#include "global.h"
u32 Script_GetValue(void);
typedef void (*Callback)(u16, u32);
Callback Script_GetActorFactory(u16);
s32 Script_CreateActorFromRegistry(void)
{
    Callback entry = Script_GetActorFactory(Script_GetValue());
    s32 result;
    if (entry) {
        entry(Script_GetValue(), 0);
        result = 0;
    } else
        result = -1;
    return result;
}
