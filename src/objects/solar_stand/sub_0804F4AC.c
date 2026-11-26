#include "objects/solar_stand/resource_station.h"
#include "ui/widgets/scene_input.h"
void ResourceStation_InitializeDigits(struct ResourceStation *station)
{
    struct SceneInputPackedPosition position;
    struct SceneInputResource *source = ScriptProc_Start(0xCB05, 0x530D);
    if (source != 0) {
        u8 *cursor, *graphics;
        s32 remaining;
        *(struct SceneInputResource *)station->digitGraphics = *source;
        cursor = station->digitGraphics;
        ResourceHeader_Relocate(cursor, source);
        {
            position.x = 0;
            position.y = 0;
            position.z = 0;
        }
        graphics = cursor;
        cursor += sizeof(struct SceneInputResource);
        remaining = 3;
        do {
            Widget_InitLinked(cursor, graphics, 0x1e, 0x31, 1, 0, 0x3c, &position);
            cursor += sizeof(struct SceneInputWidget);
            remaining--;
        } while (remaining >= 0);
    }
}
