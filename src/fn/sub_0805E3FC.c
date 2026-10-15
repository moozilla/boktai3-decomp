#include "scene_input.h"
#define SceneInput_InitMarkerPair sub_0805E3FC
void SceneInput_InitMarkerPair(struct SceneInputPair *pair)
{
    struct {
        u32 first, second;
    } position;
    struct SceneInputResource *resource = ScriptProc_Start(0xCB05, 0x5D04);
    struct SceneInputWidget *widget;
    s32 remaining;
    u32 zero;
    pair->resource = *resource;
    ResourceHeader_Relocate(pair, resource);
    {
        u32 mask = 0xFFFF0000;
        position.first = 0;
        position.second &= mask;
    }
    zero = 0;
    widget = pair->widgets;
    remaining = 1;
    do {
        sub_0821983C(widget, pair, 0, 0x31, zero, zero, 60, &position);
        widget++;
        remaining--;
    } while (remaining >= 0);
}
