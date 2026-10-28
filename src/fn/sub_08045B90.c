#include "projected_display.h"
#include "render.h"
void ProjectedDisplay_UpdateScreenPosition(struct ProjectedDisplay *display)
{
    struct ContactPosition *screen;
    struct ContactPosition *raised;
    s32 x, z, sum, vertical, y, depth;
    s32 scaledSum;
    display->raised = display->world;
    display->raised.y += display->height;
    screen = &display->screen;
    raised = &display->raised;
    x = raised->x;
    z = raised->z;
    screen->x = projectScale(3 * (x - z));
    scaledSum = projectScale(3 * (x + z));
    sum = scaledSum;
    vertical = projectScale(3 * raised->y);
    y = sum - vertical;
    depth = sum + vertical;
    screen->x = (u16)screen->x - gCameraOrigin[0] + 120;
    screen->y = y - gCameraOrigin[1] + 80;
    screen->z = depth - gCameraOrigin[2];
}
