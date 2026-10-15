#ifndef GUARD_SCRIPT_REFERENCE_H
#define GUARD_SCRIPT_REFERENCE_H
#include "global.h"
struct ScriptUtilityReference {
    u8 descriptor[4];
    u16 count, index;
};
void sub_0821AAD8(struct ScriptUtilityReference *);
void sub_0821B4C8(struct ScriptUtilityReference *, s32, s32);
#endif
