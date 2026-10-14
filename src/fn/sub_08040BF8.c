// MATCHING_CONCESSION: register-pinning
#include "modifier_list.h"
#define ModifierMenu_DrawOverview sub_08040BF8
void ModifierMenu_DrawFirstSourceStats(struct ModifierMenu *);
void ModifierMenu_DrawSecondSourceStats(struct ModifierMenu *);
void ModifierMenu_DrawThirdSourceStats(struct ModifierMenu *);
void ModifierMenu_DrawFourthSourceStats(struct ModifierMenu *);
void ModifierMenu_DrawOverview(struct ModifierMenu *menu)
{
    struct ModifierDisplayObject *object;
    u32 one;
    u32 mask;
    register s32 remaining asm("r5");
    u32 *flags;
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
        remaining = 3;
        do {
            *column |= bit;
            column = (u32 *)((u8 *)column + sizeof(struct ModifierDisplayObject));
        } while (--remaining >= 0);
    }
    {
        u32 bit = 1;
        u32 *column = &menu->displays[9].flags;
        remaining = 3;
        do {
            *column |= bit;
            column = (u32 *)((u8 *)column + sizeof(struct ModifierDisplayObject));
        } while (--remaining >= 0);
    }
    if (menu->mode == 0) {
        register s32 frame asm("r5") = 9;
        u32 zero = 0;
        struct ModifierDisplayObject *panel;
        flags = &menu->displays[9].flags;
        panel = &menu->displays[9];
        do {
            Anim_LoadKeyframe(panel, &menu->graphicsB, (u16)frame, 0);
            *flags &= ~1;
            *(s16 *)((u8 *)flags + 0x18) = 8;
            *(s16 *)((u8 *)flags + 0x1a) = zero;
            flags = (u32 *)((u8 *)flags + sizeof(struct ModifierDisplayObject));
            panel++;
            frame++;
        } while (frame <= 11);
        ModifierMenu_DrawFirstSourceStats(menu);
        ModifierMenu_DrawSecondSourceStats(menu);
        ModifierMenu_DrawThirdSourceStats(menu);
        ModifierMenu_DrawFourthSourceStats(menu);
    }
}
