// MATCHING_CONCESSION: register-pinning
#include "projected_display.h"
#include "render.h"
struct ProjectedDisplay *ProjectedDisplay_PositionEntry(struct ProjectedDisplay *display, s32 index)
{
    register struct ProjectedDisplay *result asm("r0") = display;
    struct ProjectedDisplayEntry *entry;
    entry = &display->entries[index];
    if (index == 1) {
        struct ContactPosition *destination = &entry->position;
        register struct ContactPosition *world asm("r8");
        struct ContactPosition *worldLow;
        u32 worldOffset;
        s32 x;
        s32 z;
        s32 sum, vertical, y, depth;
        s32 scaledSum;
        worldOffset = (u8 *)&display->world - (u8 *)display;
        asm("" : "+r"(worldOffset));
        worldLow = (void *)(worldOffset + (u32)display);
        world = worldLow;
        x = display->world.x;
        z = worldLow->z;
        destination->x = projectScale(3 * (x - z));
        scaledSum = projectScale(3 * (x + z));
        sum = scaledSum;
        {
            register struct ContactPosition *source asm("r3") = world;
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
    {
        const s32 *table;
        s32 offset;
        const s32 *word;
        s32 bias;
        register struct ProjectedDisplayEntry *view asm("r3");
        table = gUnk_0860594C;
        offset = index << 2;
        word = (void *)(offset + (u32)table);
        bias = *word;
        view = entry;
        view->position.z += bias;
    }
    return result;
}
