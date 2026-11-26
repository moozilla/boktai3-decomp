#ifndef GUARD_LINK_SELECTION_MODE_H
#define GUARD_LINK_SELECTION_MODE_H

#include "engine/shared/global.h"

/* A participant's selection word (struct AreaEffectLinkState selection[]) holds a mode in
 * bits 10-13 (mask 0x3C00) and a value in the low bits. Each test takes the mode mask from
 * its caller, which always passes 0x3C00. */
static inline u8 LinkSelection_IsReady(u32 value, u32 mask)
{
    if ((value & mask) == 0x1000)
        return 1;
    return 0;
}

static inline u8 LinkSelection_IsStart(u16 value, u16 mask)
{
    value &= mask;
    if (value == 0x1400)
        return 1;
    return 0;
}

static inline u8 LinkSelection_IsSelected(u32 value, u32 mask)
{
    if ((value & mask) == 0x1800)
        return 1;
    return 0;
}

static inline u8 LinkSelection_IsChoice(u32 value, u32 mask)
{
    if ((value & mask) == 0x2c00)
        return 1;
    return 0;
}

#endif
