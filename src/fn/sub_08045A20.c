#include "projected_display.h"
#include "render.h"
// No return statement: the ROM returns the caller's r0 (display) untouched (fact 95).
struct ProjectedDisplay *ProjectedDisplay_PositionEntry(struct ProjectedDisplay *display, s32 index)
{
    struct ProjectedDisplayEntry *entry = &display->entries[index];
    if (index == 1) {
        struct ContactPosition *destination = &entry->position;
        struct ContactPosition *world;
        struct ContactPosition *worldLow;
        u32 worldOffset;
        s32 x;
        s32 z;
        s32 sum, vertical, y, depth;
        s32 scaledSum;
        worldOffset = (u8 *)&display->world - (u8 *)display;
        worldLow = (void *)(worldOffset + (u32)display);
        world = worldLow;
        x = display->world.x;
        z = worldLow->z;
        destination->x = projectScale(3 * (x - z));
        scaledSum = projectScale(3 * (x + z));
        sum = scaledSum;
        {
            struct ContactPosition *source = world;
            vertical = projectScale(3 * source->y);
        }
        y = sum - vertical;
        depth = sum + vertical;
        destination->x = (u16)destination->x - gCameraOrigin[0] + 120;
        destination->y = y - gCameraOrigin[1] + 80;
        destination->z = depth - gCameraOrigin[2];
    } else {
        struct ContactPosition copy = display->screen;
        struct ProjectedDisplayEntry *view = entry;
        view->position = copy;
    }
    entry->position.z += gUnk_0860594C[index];
}