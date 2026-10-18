#include "global.h"
#define ScriptValues_TickRotation sub_0819A564
#define ScriptValues_LoadRotatedArray sub_0819A520
struct ScriptValueRotation { u8 unknown0[0x78]; s8 startIndex; u8 ticks; };
void sub_0819A520(struct ScriptValueRotation *);
s32 sub_0819A564(struct ScriptValueRotation *rotation)
{
    sub_0819A520(rotation);
    if (++rotation->ticks > 3) {
        rotation->ticks = 0;
        if (--rotation->startIndex <= 0)
            rotation->startIndex = 7;
    }
    return 0;
}
