#include "global.h"
#include "ram.h"
u32 AnimData_FindEntry(u32);
u32 ScriptProc_Start(u32, u32);
u32 SceneActorEffects_InitManager(u32 *s)
{
    gSceneActorEffectsManager = s;
    s[8] = 0;
    s[6] = AnimData_FindEntry(0x1c1a);
    s[7] = ScriptProc_Start(0x922e, 0xd1b8);
    return 0;
}
