#include "global.h"
#include "engine.h"
#include "ram.h"
#include "runtime.h"

void Engine_ClearControlFlags(void)
{
    gEngineFlags = 0;
    gScenePauseFlags = 0;
    gSceneControlWord1 = 0;
    gSceneControlWord2 = 0;
    gSceneControlWord0 = 0;
    gSceneControlWord3 = 0;
}
