// MATCHING_CONCESSION: register-pinning
#include "modifier_list.h"
#define ModifierMenu_DrawAvailableLabels sub_08041280
u32 Modifier_ReadPackedValue(u32, s32);
void sub_080339B8(s32, s32, s32, s32, s32);
s32 sub_080337FC(u32);
void ModifierMenu_DrawAvailableLabels(struct ModifierMenu *menu)
{
    u32 available = 0;
    u32 sourceIndex = 7;
    register s32 item asm("r6") = 0;
    u32 row;
    s32 y;
    const u8 **sources;
    const u8 **selectedSource;
    s32 *window;
    s8 *category;
    switch (menu->mode) {
        case 0: {
            category = &menu->choice.cursor.category;
            available = Modifier_ReadPackedValue(*category, 0);
            {
                s32 value = *category;
                sourceIndex = value + 1;
            }
            break;
        }
        case 1: {
            category = &menu->choice.cursor.category;
            available = Modifier_ReadPackedValue(4, *category);
            sourceIndex = 5;
            goto groupIndex;
        }
        case 2: {
            category = &menu->choice.cursor.category;
            available = Modifier_ReadPackedValue(5, *category);
            sourceIndex = 6;
        groupIndex: {
            s32 categoryValue = *category;
            register s32 group asm("r0") = categoryValue * 3;
            asm("" : "+r"(group));
            item = group * 2;
        } break;
        }
    }
    row = 2;
    sources = menu->textSources;
    selectedSource = &sources[sourceIndex];
    window = menu->listWindows;
    y = 5;
    do {
        u32 bit = row - 2;
        sub_080339B8(*window, 13, y, 16, 2);
        if (available & (1 << bit)) {
            TextWindow_SetScript(*window, *selectedSource);
            TextWindow_Refresh(*window, item);
        } else {
            TextWindow_SetScript(*window, *sources);
            TextWindow_Refresh(*window, 4);
        }
        sub_080337FC(*window++);
        item++;
        y += 2;
        row++;
    } while (row <= 7);
}
