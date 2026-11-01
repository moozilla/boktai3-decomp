// MATCHING_CONCESSION: register-pinning
#include "scene_input.h"
void SceneInput_InitGlyphPair(struct SceneInputPair *pair)
{
    struct {
        u32 first, second;
    } position;
    struct SceneInputResource *resource = ScriptProc_Start(0xCB05, 0x530D);
    struct SceneInputWidget *widget;
    s32 remaining;
    u32 zero;
    struct SceneInputPosition *positionPointer;
    pair->resource = *resource;
    ResourceHeader_Relocate(pair, resource);
    {
        u32 mask = 0xFFFF0000;
        position.first = 0;
        position.second &= mask;
    }
    DEBUG_NOP();
    zero = 0;
    positionPointer = (void *)&position;
    widget = pair->widgets;
    remaining = 1;
    do {
        do {
            Widget_InitLinked(widget, pair, 0x7F, 0x31, zero, zero, 60, positionPointer);
            positionPointer->y += 8;
        } while (0);
        widget++;
        remaining--;
    } while (remaining >= 0);
}
