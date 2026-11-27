#include "global.h"
u8 *Script_GetPc(void);
void GameState_BackupActiveRegions(void);
u8 *Script_BackupVariableOperand(u8 *);
s32 Script_SaveVariables(void)
{
    u8 *p = Script_GetPc();
    if (p == 0) {
        GameState_BackupActiveRegions();
    } else {
        while (*p != 0)
            p = Script_BackupVariableOperand(p);
    }
    return 0;
}
