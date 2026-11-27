#include "global.h"
#include "objects/deck_cannons/script_value_rotation_layout.h"
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
