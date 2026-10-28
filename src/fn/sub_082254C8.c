#include "global.h"
#include "runtime.h"
void SceneLifecycle_InitSceneState(void);
void Script_ResetLocalVariables(void);
s32 Script_Call(u32, void *);
void Script_RunMainBlock(void);

bool32 sub_082254C8(u8 *scene)
{
    u32 id;
    gSceneTransitionFlags = 0;
    *(u32 *)(scene + 0x1c) = 0;
    SceneLifecycle_InitSceneState();
    Script_ResetLocalVariables();
    if ((id = gScriptEntryProcedure) != 0)
        Script_Call(id, NULL);
    if (id == 0)
        Script_RunMainBlock();
    return TRUE;
}