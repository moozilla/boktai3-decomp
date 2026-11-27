#ifndef GUARD_INCLUDE_RECOVERED_SCRIPT_CALL_ARGUMENTS_LAYOUT_H
#define GUARD_INCLUDE_RECOVERED_SCRIPT_CALL_ARGUMENTS_LAYOUT_H

#include "global.h"

struct ScriptCallArguments {
    u32 count : 16; /* confidence=low */
    u32 pad : 16; /* confidence=low */
    s32 *values; /* confidence=low */
};

typedef char ScriptCallArgumentsSizeCheck[sizeof(struct ScriptCallArguments) == 0x8 ? 1 : -1];

#endif
