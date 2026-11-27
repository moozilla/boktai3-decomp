#include "ui/widgets/scene_input.h"
void SceneInput_InitMarkerPair(struct SceneInputPair *pair)
{
    struct ContactPosition position;
    struct AnimSpriteResource *resource = ScriptProc_Start(0xCB05, 0x5D04);
    s32 i;
    pair->resource = *resource;
    ResourceHeader_Relocate(pair, resource);
    {
        position.x = 0;
        position.y = 0;
        position.z = 0;
    }
    for (i = 0; i < 2; i++)
        AnimSprite_InitLinked(&pair->widgets[i], pair, 0, 0x31, 0, 0, 60, &position);
}
