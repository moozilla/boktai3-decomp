#include "ui/widgets/scene_input.h"
void SceneInput_InitGlyphPair(struct SceneInputPair *pair)
{
    struct ContactPosition position;
    struct AnimSpriteResource *resource = ScriptProc_Start(0xCB05, 0x530D);
    struct AnimSprite *widget;
    s32 i;
    pair->resource = *resource;
    ResourceHeader_Relocate(pair, resource);
    {
        position.x = 0;
        position.y = 0;
        position.z = 0;
    }
    for (i = 0; i < 2; i++) {
        widget = &pair->widgets[i];
        Widget_InitLinked(widget, pair, 0x7F, 0x31, 0, 0, 60, &position);
        position.y += 8;
    }
}
