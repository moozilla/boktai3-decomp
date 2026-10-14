// MATCHING_CONCESSION: register-pinning
#include "modifier_list.h"
#define ModifierMenu_DrawSelectedLabels sub_08041128
u32 Modifier_TestSelection(struct ModifierSelection *, s32);
void sub_080339B8(s32, s32, s32, s32, s32);
s32 sub_080337FC(u32);
void ModifierMenu_DrawSelectedLabels(struct ModifierMenu *menu)
{
    struct ModifierSelection *current = &menu->current;
    s32 *window;
    s32 count;
    register s32 id asm("r6");
    s32 y;
    switch (menu->mode) {
        case 0:
            if (menu->state != 2)
                break;
            window = &menu->detailWindow;
            sub_080339B8(*window, 1, 6, 28, 6);
            TextWindow_SetScript(*window, menu->textSources[0]);
            TextWindow_Refresh(*window, 7);
            sub_080337FC(*window);
            break;
        case 1:
            count = 0;
            id = 0;
            window = menu->listWindows;
            y = 4;
        selectedLoop:
            if (!Modifier_TestSelection(current, id))
                goto nextId;
            sub_080339B8(*window, 13, y, 16, 2);
            TextWindow_SetScript(*window, menu->textSources[5]);
            TextWindow_Refresh(*window, id);
            sub_080337FC(*window++);
            y += 2;
            count++;
            {
                register struct ModifierSelection *base asm("r1") = current;
                s32 selectedCount;
                selectedCount = base->count;
                if (count >= selectedCount)
                    goto collected;
            }
        nextId:
            id++;
            if (id <= 31)
                goto selectedLoop;
        collected:
            id = count;
            {
                s32 capacity;
                {
                    register struct ModifierSelection *base asm("r1") = current;
                    capacity = base->capacity;
                }
                if (id < capacity) {
                    register s32 row asm("r5");
                    window = (s32 *)(id * sizeof(*window) + (u32)menu->listWindows);
                    {
                        register s32 rowBase asm("r0") = id * 2;
                        row = rowBase + 4;
                    }
                    do {
                        sub_080339B8(*window, 13, row, 16, 2);
                        asm("" : "+r"(menu));
                        TextWindow_SetScript(*window, menu->textSources[0]);
                        TextWindow_Refresh(*window, 5);
                        sub_080337FC(*window++);
                        row += 2;
                        id++;
                        {
                            struct ModifierSelection *base = current;
                            capacity = base->capacity;
                        }
                    } while (id < capacity);
                }
            }
            break;
        case 2:
            id = 0;
            do {
                count = current->secondaryId;
                if (count >= 0) {
                    window = menu->listWindows;
                    sub_080339B8(*window, 13, 4, 16, 2);
                    TextWindow_SetScript(*window, menu->textSources[6]);
                    TextWindow_Refresh(*window, count);
                    sub_080337FC(*window);
                }
            } while (--id >= 0);
            break;
    }
}
