#include "global.h"

void ScriptTriggerActor_DispatchPendingScripts(void);

void Pool1B8_RunPendingScripts(void)
{
    ScriptTriggerActor_DispatchPendingScripts();
}
