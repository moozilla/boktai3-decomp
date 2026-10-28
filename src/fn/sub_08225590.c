#include "global.h"
#include "script_virtual_machine.h"
u32 Script_GetActorFactory(u16);
u32 sub_08249238(void);
s32 Script_InvokeRegistryCommand(void)
{
    if (Script_GetActorFactory(Script_GetValue()) == 0)
        return -1;
    gScriptVm.result = sub_08249238();
    return 0;
}
