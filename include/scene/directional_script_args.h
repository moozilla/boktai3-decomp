#ifndef GUARD_DIRECTIONAL_SCRIPT_ARGS_H
#define GUARD_DIRECTIONAL_SCRIPT_ARGS_H
#include "global.h"
struct DirectionalScriptArgs {
    u32 count : 16, unused : 16;
    s32 *values;
};
#endif
