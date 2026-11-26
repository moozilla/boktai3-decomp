#include "ui/hud/stage_icon_display.h"
#include "objects/scene_objects/scene_object.h"
void StageIconDisplay_InitAnimations(struct StageIconDisplay *display, s32 variant)
{
    struct ProjectedDisplayResource *resource;
    resource = ScriptProc_Start(0xCB05, 0x1289);
    if (resource) {
        struct SceneExtent position;
        struct ProjectedDisplayResource *header;
        struct ProjectedDisplayResource *savedHeader;
        display->resource = *resource;
        header = &display->resource;
        ResourceHeader_Relocate(header, resource);
        position.x = 0;
        position.y = 0;
        position.z = 0;
        savedHeader = header;
        if (variant == 0) {
            s32 i;
            Widget_InitLinked(&display->icon, header, 48, 48, 0, 0, 60, &position);
            for (i = 0; i < 3; i++) {
                Widget_InitLinked(&display->indicators[i], savedHeader, 49, 49, 0, 0, 60,
                                  &position);
                position.x += 8;
            }
        } else if (variant == 1) {
            Widget_InitLinked(&display->icon, header, 51, 48, 0, 0, 60, &position);
        } else {
            Widget_InitLinked(&display->icon, header, 48, 48, 0, 0, 60, &position);
        }
    }
}
