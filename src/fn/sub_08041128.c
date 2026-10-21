#include "modifier_list.h"
u32 Modifier_TestSelection(struct ModifierSelection *, s32);
void TextWindow_SetBounds(s32, s32, s32, s32, s32);
s32 TextWindow_StartPlayback(u32);
void ModifierMenu_DrawSelectedLabels(struct ModifierMenu *menu)
{
    struct ModifierSelection *current = &menu->current;
    s32 *window;
    s32 count;
    s32 id;
    s32 y;
    switch (menu->mode) {
        case 0:
            if (menu->state != 2) {
                break;
            }
            window = &menu->detailWindow;
            TextWindow_SetBounds(*window, 1, 6, 28, 6);
            TextWindow_SetScript(*window, menu->textSources[0]);
            TextWindow_Refresh(*window, 7);
            TextWindow_StartPlayback(*window);
            break;
        case 1:
            count = 0;
            id = 0;
            window = menu->listWindows;
            y = 4;
        selectedLoop:
            if (!Modifier_TestSelection(current, id)) {
                goto nextId;
            }

            TextWindow_SetBounds(*window, 13, y, 16, 2);
            TextWindow_SetScript(*window, menu->textSources[5]);
            TextWindow_Refresh(*window, id);
            TextWindow_StartPlayback(*window++);
            y += 2;
            count++;
            {
                struct ModifierSelection *base = current;
                s32 selectedCount;
                selectedCount = base->count;
                if (count >= selectedCount) {
                    goto collected;
                }
            }
        nextId:
            id++;

            if (id <= 31) {
                goto selectedLoop;
            }
        collected:
            id = count;
            {
                s32 capacity;
                {
                    struct ModifierSelection *base = current;
                    capacity = base->capacity;
                }
                if (id < capacity) {
                    s32 row;
                    window = (s32 *)(id * sizeof(*window) + (u32)menu->listWindows);
                    {
                        s32 rowBase = id * 2;
                        row = rowBase + 4;
                    }
                    do {
                        TextWindow_SetBounds(*window, 13, row, 16, 2);
                        TextWindow_SetScript(*window, (*(&menu))->textSources[0]);
                        TextWindow_Refresh(*window, 5);
                        TextWindow_StartPlayback(*window++);
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
                    TextWindow_SetBounds(*window, 13, 4, 16, 2);
                    TextWindow_SetScript(*window, menu->textSources[6]);
                    TextWindow_Refresh(*window, count);
                    TextWindow_StartPlayback(*window);
                }
            } while (--id >= 0);
            break;
    }
}
