#include "ui/hud/stage_icon_display.h"
#include "objects/scene_object.h"
void StageIconDisplay_InitAnimations(struct StageIconDisplay *display, s32 variant)
{
    struct ProjectedDisplayResource *resource = ScriptProc_Start(0xCB05, 0x1289);

    if (resource) {
        struct SceneExtent position;

        display->resource = *resource;
        ResourceHeader_Relocate(&display->resource, resource);
        position.x = 0;
        position.y = 0;
        position.z = 0;
        if (variant == 0) {
            s32 i;

            AnimSprite_InitLinked(&display->icon, &display->resource, 48, 48, 0, 0, 60, &position);
            for (i = 0; i < 3; i++) {
                AnimSprite_InitLinked(&display->indicators[i], &display->resource, 49, 49, 0, 0, 60,
                                      &position);
                position.x += 8;
            }
        } else if (variant == 1) {
            AnimSprite_InitLinked(&display->icon, &display->resource, 51, 48, 0, 0, 60, &position);
        } else {
            AnimSprite_InitLinked(&display->icon, &display->resource, 48, 48, 0, 0, 60, &position);
        }
    }
}
