#ifdef NONMATCHING
// fix.py best (sweep after the inlined-insn fix)
// Remaining differences are low-register allocation; reviewed full extent is 716 bytes.
#include "modifier_list.h"
void ModifierMenu_DrawFirstSourceStats(struct ModifierMenu *);
void ModifierMenu_DrawSecondSourceStats(struct ModifierMenu *);
void ModifierMenu_DrawThirdSourceStats(struct ModifierMenu *);
void ModifierMenu_DrawFourthSourceStats(struct ModifierMenu *);
u32 Modifier_TestSelection(struct ModifierSelection *, s32);
void ModifierMenu_PositionSelectionMarker(struct ModifierDisplayObject *, u32);
s32 Mod(s32, s32);
void ModifierMenu_DrawPendingChoice(struct ModifierMenu *menu)
{
    struct ModifierSelection *current = &menu->current;
    struct ModifierDisplayObject *object;
    u32 one;
    u32 mask;
    s32 remaining;
    s32 mode;
    menu->previewFlags |= 1;
    one = 1;
    object = &menu->displays[8];
    Anim_LoadKeyframe(object, &menu->graphicsB, 29, 0);
    object->flags &= ~1;
    object->y = menu->choice.cursor.index << 4;
    object = &menu->displays[13];
    object->flags |= one;
    mask = ~1;
    object = &menu->displays[5];
    object->flags = mask & object->flags;
    object = &menu->displays[6];
    object->flags = mask & object->flags;
    object = &menu->displays[7];
    object->flags = mask & object->flags;
    {
        u32 bit = 1;
        u32 *column = &menu->displays[9].flags;
        remaining = 3;
        do {
            *column |= bit;
            column = (u32 *)(((u8 *)column) + (sizeof(struct ModifierDisplayObject)));
        } while ((--remaining) >= 0);
    }
    {
        u32 bit = 1;
        u32 *column = &menu->displays[14].flags;
        remaining = 3;
        do {
            *column |= bit;
            column = (u32 *)(((u8 *)column) + (sizeof(struct ModifierDisplayObject)));
        } while ((--remaining) >= 0);
    }
    mode = menu->mode;
    switch (mode) {
        case 0: {
            s32 selectedCategory = menu->choice.cursor.category;
            s8 *category;
            category = &menu->choice.cursor.category;
            if (selectedCategory == 0) {
                object = &menu->displays[9];
                object->x = -96;
                object->y = 72;
                Anim_LoadKeyframe(object, &menu->graphicsB, 9, 0);
                ModifierMenu_DrawFirstSourceStats(menu);
            } else if (selectedCategory == 1) {
                object = &menu->displays[10];
                {
                    typeof(object->x) *q1 = &object->x;
                    u8 z1 = 56;
                    *q1 = -96;
                    object->y = z1;
                    Anim_LoadKeyframe(object, &menu->graphicsB, 10, 0);
                    ModifierMenu_DrawSecondSourceStats(menu);
                }
            } else if (selectedCategory == 2) {
                object = &menu->displays[11];
                object->x = -96;
                object->y = 24;
                Anim_LoadKeyframe(object, &menu->graphicsB, 11, 0);
                ModifierMenu_DrawThirdSourceStats(menu);
            } else {
                object = &menu->displays[12];
                object->x = mode;
                object->y = mode;
                Anim_LoadKeyframe(object, &menu->graphicsB, 12, 0);
                ModifierMenu_DrawFourthSourceStats(menu);
            }
            object->flags &= ~1;
            object = &menu->displays[14];
            if ((*category) == 3) {
                if (menu->current.sourceIds[3] < 0)
                    object->flags |= 1;
                else {
                    object->flags &= ~1;
                    ModifierMenu_PositionSelectionMarker(object, current->sourceIds[*category]);
                }
            } else {
                object->flags &= ~1;
                ModifierMenu_PositionSelectionMarker(object, current->sourceIds[*category]);
            }
            break;
        }

        case 1: {
            s32 start;
            s32 end;
            s32 id;
            object = &menu->displays[10];
            object->x = -96;
            object->y = 56;
            ModifierMenu_DrawSecondSourceStats(menu);
            object->flags &= ~1;
            object = &menu->displays[14];
            start = (menu->choice.cursor.category * 3) * 2;
            end = start + 6;
            for (id = start; id < end; id++) {
                if (Modifier_TestSelection(current, id)) {
                    object->flags &= ~1;
                    ModifierMenu_PositionSelectionMarker(object, Mod(id, 6));
                    object++;
                }
            }

            break;
        }

        case 2: {
            s32 start;
            s32 end;
            object = &menu->displays[10];
            object->x = -96;
            object->y = 56;
            ModifierMenu_DrawSecondSourceStats(menu);
            object->flags &= ~1;
            object = &menu->displays[14];
            start = (menu->choice.cursor.category * 3) * 2;
            end = start + 6;
            if ((current->secondaryId >= start) && (current->secondaryId < (start + 6))) {
                object->flags &= ~1;
                ModifierMenu_PositionSelectionMarker(object, Mod(current->secondaryId, 6));
            }
            break;
        }
    }
}

#else
#include "global.h"
INCLUDE_ASM("asm/nonmatching", ModifierMenu_DrawPendingChoice);
#endif
