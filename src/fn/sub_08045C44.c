#include "projected_display.h"
#define ProjectedDisplay_Initialize           sub_08045C44
#define ProjectedDisplay_UpdateScreenPosition sub_08045B90
#define ProjectedDisplay_InitializeEntries    sub_08045B08
void ProjectedDisplay_UpdateScreenPosition(struct ProjectedDisplay *);
s32 ProjectedDisplay_InitializeEntries(struct ProjectedDisplay *);
s32 ProjectedDisplay_Initialize(struct ProjectedDisplay *display,
                                const struct ContactPosition *position)
{
    struct ProjectedDisplayResource *resource;
    s32 result;
    display->height = 0;
    display->world = *position;
    ProjectedDisplay_UpdateScreenPosition(display);
    resource = ScriptProc_Start(0xCB05, 0x7516);
    if (!resource)
        goto failed;
    {
        display->resource = *resource;
        ResourceHeader_Relocate(&display->resource, resource);
        display->world = *position;
        display->raised = *position;
        ProjectedDisplay_InitializeEntries(display);
        result = 0;
        goto done;
    }
failed:
    result = -1;
done:
    return result;
}
