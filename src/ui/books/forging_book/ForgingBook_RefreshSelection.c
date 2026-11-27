#include "global.h"
void ForgingBook_UpdateSelectionCursor(u32);
void ForgingBook_UpdateCategoryCountDigits(u32);
void ForgingBook_RefreshSelectedEntryText(u32);
u32 ForgingBook_RefreshSelection(u32 a)
{
    ForgingBook_UpdateSelectionCursor(a);
    ForgingBook_UpdateCategoryCountDigits(a);
    ForgingBook_RefreshSelectedEntryText(a);
    return 0;
}
