#include "global.h"
#define SelectionList_SetUpdate       SelectionCursor_SetState
#define SelectionList_UpdateSelection MenuCursor_UpdateInput
#define gCursorSineTable              gUnk_086149C4
struct SelectionList {
    u8 unknown0[0x38];
    s16 cursorX, cursorY;
    u8 unknown3C[0x5C];
    s16 savedX, savedY;
    u8 unknown9C[0x110];
    s32 frame;
};
extern const s16 gUnk_086149C4[];
void SelectionCursor_SetState(void *, void (*)(void));
void MenuCursor_UpdateInput(void);
void SelectionList_TickCursorShake(struct SelectionList *list)
{
    s16 *cursor = &list->cursorX;
    s32 value = gCursorSineTable[((list->frame << 7) + 0x40) & 0xFF] * 4;
    s32 offset;
    if (value >= 0)
        offset = value >> 13;
    else
        offset = -((-value) >> 13);
    cursor[0] = list->savedX + offset;
    cursor[1] = list->savedY;
    if (++list->frame > 2)
        SelectionList_SetUpdate(list, SelectionList_UpdateSelection);
}
