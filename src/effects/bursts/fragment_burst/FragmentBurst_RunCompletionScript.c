#include "effects/bursts/fragment_burst.h"

void FragmentBurst_RunCompletionScript(struct FragmentBurst *burst)
{
    if (burst->completionScript)
        Script_Call(burst->completionScript, 0);
}
