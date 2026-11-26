#include "global.h"
u8 *Script_GetPc(void);
u32 Script_ReadBackupVariableOperand(u8 *);
u32 Script_GetSavedVariable(void)
{
    return Script_ReadBackupVariableOperand(Script_GetPc());
}
