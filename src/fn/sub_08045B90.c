#include "projected_display.h"
extern u16 gUnk_03005418[];
#define ProjectedDisplay_UpdateScreenPosition sub_08045B90
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
    screen->x = (u16)screen->x - gUnk_03005418[0] + 120;
    screen->y = y - gUnk_03005418[1] + 80;
    screen->z = depth - gUnk_03005418[2];
}
