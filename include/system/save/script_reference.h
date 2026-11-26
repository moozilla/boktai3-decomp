#ifndef GUARD_SCRIPT_REFERENCE_H
#define GUARD_SCRIPT_REFERENCE_H
#include "global.h"
struct ScriptUtilityReference {
    u8 descriptor[4];
    u16 count, index;
};
void Script_ReadReference(struct ScriptUtilityReference *);
void Script_WriteReferenceValue(struct ScriptUtilityReference *, s32, s32);
#endif
