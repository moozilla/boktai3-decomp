#include "global.h"
void Script_ResetStacks(void);
void GameState_ClearActiveRegions(void);
void Script_ClearCommandTables(void);
void Script_RegisterBuiltinCommands(void);
void ScriptProc_Start(u32, u32);
void Script_LoadScript(void);
void Script_SetEntryProcedure(u32);
void Script_InitRuntime(void)
{
    Script_ResetStacks();
    GameState_ClearActiveRegions();
    Script_ClearCommandTables();
    Script_RegisterBuiltinCommands();
    ScriptProc_Start(0xA8D9, 0x49BC);
    Script_LoadScript();
    Script_SetEntryProcedure(0);
}
