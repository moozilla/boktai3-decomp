#include "scene_input.h"
void SceneInput_InitGlyphPair(struct SceneInputPair *pair)
{
    struct SceneInputPackedPosition position;
    struct SceneInputResource *resource = ScriptProc_Start(0xCB05, 0x530D);
    struct SceneInputWidget *widget;
    s32 i;
    u32 zero;
    struct SceneInputPosition *positionPointer;
    pair->resource = *resource;
    ResourceHeader_Relocate(pair, resource);
    {
        position.x = 0;
        position.y = 0;
        position.z = 0;
    }
    zero = 0;
    i = 0;
    do {
        positionPointer = (void *)&position;
        widget = &pair->widgets[i];
        Widget_InitLinked(widget, pair, 0x7F, 0x31, zero, zero, 60, positionPointer);
        positionPointer->y += 8;
        i++;
    } while (i <= 1);
}
