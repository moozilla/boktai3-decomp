#include "engine/shared/global.h"
#include "engine/shared/engine.h"
#include "engine/shared/ram.h"
#include "engine/shared/runtime.h"

void Engine_ClearControlFlags(void)
{
    gEngineFlags = 0;
    gScenePauseFlags = 0;
    gSceneControlWord1 = 0;
    gSceneControlWord2 = 0;
    gSceneControlWord0 = 0;
    gSceneControlWord3 = 0;
}
