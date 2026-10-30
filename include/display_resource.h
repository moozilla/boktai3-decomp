#ifndef GUARD_DISPLAY_RESOURCE_H
#define GUARD_DISPLAY_RESOURCE_H

#include "global.h"

/* Script-loaded display resources copy this header before rebasing words 3..7. */
struct DisplayResourceHeader {
    u32 words[8];
};

s32 ResourceHeader_Relocate(void *destination, void *source);

#endif
