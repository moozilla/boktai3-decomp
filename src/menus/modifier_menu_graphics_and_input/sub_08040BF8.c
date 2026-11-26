#include "engine/shared/modifier_list.h"
void ModifierMenu_DrawFirstSourceStats(struct ModifierMenu *);
void ModifierMenu_DrawSecondSourceStats(struct ModifierMenu *);
void ModifierMenu_DrawThirdSourceStats(struct ModifierMenu *);
void ModifierMenu_DrawFourthSourceStats(struct ModifierMenu *);
void ModifierMenu_DrawOverview(struct ModifierMenu *menu)
{
    struct ModifierDisplayObject *object;
    u32 mask;
    s32 index;
    {
        u32 value = menu->previewFlags;
        mask = ~1;
        menu->previewFlags = value & mask;
    }
    object = &menu->displays[8];
    {
        u32 value = object->flags;
        object->flags = value | 1;
    }
    object = &menu->displays[13];
    object->flags &= mask;
    object = &menu->displays[5];
    object->flags |= 1;
    object = &menu->displays[6];
    object->flags |= 1;
    object = &menu->displays[7];
    object->flags |= 1;
    for (index = 14; index < 18; index++)
        menu->displays[index].flags |= 1;
    for (index = 9; index < 13; index++)
        menu->displays[index].flags |= 1;
    if (menu->mode == 0) {
        for (index = 9; index <= 11; index++) {
            struct ModifierDisplayObject *panel = &menu->displays[index];
            Anim_LoadKeyframe(panel, &menu->graphicsB, (u16)index, 0);
            panel->flags &= ~1;
            panel->x = 8;
            panel->y = 0;
        }
        ModifierMenu_DrawFirstSourceStats(menu);
        ModifierMenu_DrawSecondSourceStats(menu);
        ModifierMenu_DrawThirdSourceStats(menu);
        ModifierMenu_DrawFourthSourceStats(menu);
    }
}
