#include "global.h"
#include "engine.h"
#include "ram.h"
#include "runtime.h"

extern u32 gUnk_03005410, gUnk_03005414;

void Engine_ClearControlFlags(void)
{
    gEngineFlags = 0;
    gScenePauseFlags = 0;
    gSceneControlWord1 = 0;
    gUnk_03005410 = 0;
    gSceneControlWord0 = 0;
    gUnk_03005414 = 0;
}
