#include "scene/actor/modifier_list.h"
void ModifierMenu_DrawCapacity(s32, s32, s32, s32, s32);
void sub_08215EB4(s32, s32, s32, s32, s32, s32, s32, s32);
static inline void drawCapacity(struct ModifierSelection *current)
{
    s32 count = current->count;
    s32 capacity;
    capacity = current->capacity;
    ModifierMenu_DrawCapacity(count, capacity, capacity, 8, 16);
}
void ModifierMenu_UpdateCapacityFlash(struct ModifierMenu *menu)
{
    if (menu->state == 1 && menu->mode == 1) {
        if (menu->errorFlash) {
            struct ModifierSelection *current = &menu->current;
            if ((menu->errorFlash & 15) <= 7)
                drawCapacity(current);
            else
                sub_08215EB4(2, 8, 16, 4, 1, 1, 0, 0);
            menu->errorFlash--;
            if (menu->errorFlash == 0)
                drawCapacity(current);
        }
    } else {
        menu->errorFlash = 0;
    }
}
