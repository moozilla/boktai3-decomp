#include "global.h"
#include "script_virtual_machine.h"
u32 Script_GetActorFactory(u16);
u32 sub_08249238(u32);
s32 Script_InvokeRegistryCommand(void)
{
    u32 factory = Script_GetActorFactory(Script_GetValue());
    if (factory == 0)
        return -1;
    gScriptVm.result = sub_08249238(factory);
    return 0;
}
