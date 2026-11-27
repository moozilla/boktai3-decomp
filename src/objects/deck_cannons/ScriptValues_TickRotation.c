#include "global.h"
struct ScriptValueRotation {
    u8 unknown0[0x78];
    s8 startIndex;
    u8 ticks;
};
void ScriptValues_LoadRotatedArray(struct ScriptValueRotation *);
s32 ScriptValues_TickRotation(struct ScriptValueRotation *rotation)
{
    ScriptValues_LoadRotatedArray(rotation);
    if (++rotation->ticks > 3) {
        rotation->ticks = 0;
        if (--rotation->startIndex <= 0)
            rotation->startIndex = 7;
    }
    return 0;
}
