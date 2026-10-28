#include "global.h"
#include "render.h"
struct ScriptValueRotation {
    u8 unknown0[0x78];
    s8 startIndex;
};
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
    u16 *value = source->values;
    s32 remaining = 6;
    do {
        if (index > 7)
            index -= 7;
        destination[index] = *value;
        value++;
        remaining--;
        index++;
    } while (remaining >= 0);
}
