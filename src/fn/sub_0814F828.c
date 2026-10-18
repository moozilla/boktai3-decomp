#include "global.h"
void ScriptActor_EnableCompletionScript(u8 *p)
{
    p[0x59e] = 1;
}
