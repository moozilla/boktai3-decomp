// MATCHING_CONCESSION: register-pinning
#include "stage_icon_display.h"
#include "scene_object.h"
#define StageIconDisplay_InitAnimations sub_08055DD4
void StageIconDisplay_InitAnimations(struct StageIconDisplay *argument, s32 variantArgument)
{
    struct StageIconDisplay *display = argument;
    register s32 variant asm("r4");
    struct ProjectedDisplayResource *resource;
    variant = variantArgument;
    resource = ScriptProc_Start(0xCB05, 0x1289);
    if (resource) {
        struct SceneExtent position;
        register struct ProjectedDisplayResource *header asm("r5");
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
            register struct SceneExtent *point asm("r4");
            scratch = (u32)&position;
            sub_0821983C(&display->icon, header, 48, 48, variant, variant, 60, (void *)scratch);
            scratch = 0;
            point = &position;
            indicator = (void *)((u8 *)header + 0x80);
            i = 2;
            do {
                sub_0821983C(indicator, savedHeader, 49, 49, scratch, scratch, 60, point);
                point->x += 8;
                indicator++;
            } while (--i >= 0);
        } else if (variant == 1)
            sub_0821983C(&display->icon, header, 51, 48, 0, 0, 60, &position);
        else
            sub_0821983C(&display->icon, header, 48, 48, 0, 0, 60, &position);
    }
}
