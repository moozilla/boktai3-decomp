#include "stage_icon_display.h"
#include "scene_object.h"
void StageIconDisplay_InitAnimations(struct StageIconDisplay *argument, s32 variantArgument)
{
    struct StageIconDisplay *display = argument;
    s32 variant;
    struct ProjectedDisplayResource *resource;
    variant = variantArgument;
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
            struct ProjectedDisplayEntry *indicator;
            s32 i;
            u32 scratch;
            struct SceneExtent *point;
            do {
            scratch = (u32)&position;
            } while (0);
            Widget_InitLinked(&display->icon, header, 48, 48, variant, variant, 60,
                              (void *)scratch);
            do {
            scratch = 0;
            point = &position;
            } while (0);
            indicator = (void *)((u8 *)header + 0x80);
            i = 2;
            do {
                Widget_InitLinked(indicator, savedHeader, 49, 49, scratch, scratch, 60, point);
                point->x += 8;
                indicator++;
            } while (--i >= 0);
        } else if (variant == 1) {
            Widget_InitLinked(&display->icon, header, 51, 48, 0, 0, 60, &position);
        } else {
            Widget_InitLinked(&display->icon, header, 48, 48, 0, 0, 60, &position);
        }
    }
}
