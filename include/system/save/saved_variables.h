#ifndef GUARD_SAVED_VARIABLES_H
#define GUARD_SAVED_VARIABLES_H

#include "global.h"
#include "system/save/script_reference.h"

/* Heap buffer holding script variable values while the saved regions are reset. */
struct SavedVariables {
    u32 count;
    struct ScriptUtilityReference references[512]; /* 0x4 */
    u32 values[512];                               /* 0x1004 */
};

typedef char SavedVariablesSizeCheck[sizeof(struct SavedVariables) == 0x1804 ? 1 : -1];

#endif
