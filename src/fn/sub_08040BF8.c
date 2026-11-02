#include "modifier_list.h"
void ModifierMenu_DrawFirstSourceStats(struct ModifierMenu *);
void ModifierMenu_DrawSecondSourceStats(struct ModifierMenu *);
void ModifierMenu_DrawThirdSourceStats(struct ModifierMenu *);
void ModifierMenu_DrawFourthSourceStats(struct ModifierMenu *);
/* A cursor starting at a display object's flags reaches x and y at +0x18 and +0x1A. */
struct ModifierDisplayFields {
    u32 flags;
    u8 beforePosition[0x14];
    s16 x, y;
};
void ModifierMenu_DrawOverview(struct ModifierMenu *menu)
{
    struct ModifierDisplayObject *object;
    u32 one;
    u32 mask;
    s32 index;
    struct ModifierDisplayFields *fields;
    {
        u32 value = menu->previewFlags;
        mask = ~1;
        menu->previewFlags = value & mask;
    }
    object = &menu->displays[8];
    {
        u32 value = object->flags;
        one = 1;
        object->flags = value | one;
    }
    object = &menu->displays[13];
    object->flags &= mask;
    object = &menu->displays[5];
    object->flags |= one;
    object = &menu->displays[6];
    object->flags |= one;
    object = &menu->displays[7];
    object->flags |= one;
    {
        u32 bit = 1;
        u32 *column = &menu->displays[14].flags;
        index = 3;
        do {
            *column |= bit;
            column = (u32 *)((u8 *)column + sizeof(struct ModifierDisplayObject));
        } while (--index >= 0);
    }
    {
        u32 bit = 1;
        u32 *column = &menu->displays[9].flags;
        index = 3;
        do {
            *column |= bit;
            column = (u32 *)((u8 *)column + sizeof(struct ModifierDisplayObject));
        } while (--index >= 0);
    }
    if (menu->mode == 0) {
        u32 zero;
        struct ModifierDisplayObject *panel;
        index = 9;
        zero = 0;
        fields = (void *)&menu->displays[9].flags;
        panel = &menu->displays[9];
        do {
            Anim_LoadKeyframe(panel, &menu->graphicsB, (u16)index, 0);
            fields->flags &= ~1;
            fields->x = 8;
            fields->y = zero;
            fields = (void *)((u8 *)fields + sizeof(*panel));
            panel++;
            index++;
        } while (index <= 11);
        ModifierMenu_DrawFirstSourceStats(menu);
        ModifierMenu_DrawSecondSourceStats(menu);
        ModifierMenu_DrawThirdSourceStats(menu);
        ModifierMenu_DrawFourthSourceStats(menu);
    }
}
