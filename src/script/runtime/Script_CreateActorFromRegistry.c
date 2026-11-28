#include "global.h"
u32 Script_GetValue(void);
typedef void (*Callback)(u16, u32);
Callback Script_GetActorFactory(u16);
s32 Script_CreateActorFromRegistry(void)
{
    Callback entry = Script_GetActorFactory(Script_GetValue());
    if (!entry)
        return -1;
    entry(Script_GetValue(), 0);
    return 0;
}
