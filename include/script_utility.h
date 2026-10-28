#ifndef GUARD_SCRIPT_UTILITY_H
#define GUARD_SCRIPT_UTILITY_H
#include "engine.h"
#include "script_reference.h"
struct ScriptUtilityArguments {
    u32 count : 16;
    u32 unused : 16;
    s32 *values;
};
extern u16 gPadState[];
s32 Script_ReadValue(const void *);
u32 Script_RunBlock(u32, struct ScriptUtilityArguments *);
void ScriptUtility_SetControlFlag(s32);
void ScriptUtility_ClearControlFlag(void);
#endif
