#include "scene_input.h"
#define SceneInput_MapGlyphIndex sub_0805E30C
s32 SceneInput_MapGlyphIndex(s32 value)
{
    if (value)
        value += 0x80;
    else
        value = 0x7F;
    return value;
}
