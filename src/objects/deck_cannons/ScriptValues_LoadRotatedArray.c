#include "global.h"
#include "engine/video/render.h"
#include "objects/deck_cannons/script_value_rotation_layout.h"
struct ScriptValueSource {
    u8 unknown0[0x56];
    u16 values[7];
};
void *ScriptProc_Start(u16, u16);
void ScriptValues_LoadRotatedArray(struct ScriptValueRotation *rotation)
{
    struct ScriptValueSource *source = ScriptProc_Start(0x92b3, 0x26bb);
    u16 *destination = gBgPaletteBank2;
    s32 index = rotation->startIndex;
    s32 i;

    for (i = 0; i < 7; i++, index++) {
        if (index > 7)
            index -= 7;
        destination[index] = source->values[i];
    }
}
