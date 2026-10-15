#include "hit_switch.h"
#define HitSwitch_RegisterCompletedCell sub_080542A4
void sub_0821D698(struct SceneCellRef *, u32, u32, s32, u32, u32);
void HitSwitch_RegisterCompletedCell(struct HitSwitchEntry *entry)
{
    u32 kind;
    s32 height;
    if (entry->kind == 0) {
        kind = (entry->sprite.flags & 4) ? 2 : 1;
        {
            s32 y = entry->world.y;
            if (y >= 0)
                y = y >> 8;
            else
                y = -(-y >> 8);
            height = y + 1;
        }
    } else {
        kind = 0;
        {
            s32 y = entry->world.y;
            if (y >= 0)
                height = y >> 8;
            else
                height = -(-y >> 8);
        }
    }
    sub_0821D698(&entry->cellRef, entry->cellIndex, kind, height, 255, 0);
    entry->cellRegistered = 1;
}
