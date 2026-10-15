// MATCHING_CONCESSION: register-pinning
#include "scene_input.h"
#define SceneInput_InitGlyphPair sub_0805E270
void SceneInput_InitGlyphPair(struct SceneInputPair *pair)
{
    struct {
        u32 first, second;
    } position;
    struct SceneInputResource *resource = ScriptProc_Start(0xCB05, 0x530D);
    struct SceneInputWidget *widget;
    s32 remaining;
    u32 zero;
    register struct SceneInputPosition *positionPointer asm("r4");
    pair->resource = *resource;
    ResourceHeader_Relocate(pair, resource);
    {
        u32 mask = 0xFFFF0000;
        position.first = 0;
        position.second &= mask;
    }
    zero = 0;
    positionPointer = (void *)&position;
    widget = pair->widgets;
    remaining = 1;
    do {
        sub_0821983C(widget, pair, 0x7F, 0x31, zero, zero, 60, positionPointer);
        positionPointer->y += 8;
        widget++;
        remaining--;
    } while (remaining >= 0);
}
