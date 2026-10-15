#ifndef GUARD_SCRIPT_UTILITY_H
#define GUARD_SCRIPT_UTILITY_H
#include "engine.h"
struct ScriptUtilityReference {
    u8 descriptor[4];
    u16 count, index;
};
struct ScriptUtilityArguments {
    u32 count : 16;
    u32 unused : 16;
    s32 *values;
};
extern u16 gUnk_03005260[];
void sub_0821AAD8(struct ScriptUtilityReference *);
void sub_0821B4C8(struct ScriptUtilityReference *, s32, s32);
s32 sub_0821AA00(const void *);
u32 sub_0821AFB4(u32, struct ScriptUtilityArguments *);
void ScriptUtility_SetControlFlag(s32);
void ScriptUtility_ClearControlFlag(void);
#endif
