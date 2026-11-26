#include "global.h"
void CollectionBook_UpdateSelectionCursor(u32);
void CollectionBook_RefreshCompletionDigits(u32);
void ForgingBook_RefreshEntryDescription(u32);
u32 sub_0821353C(u32 a)
{
    CollectionBook_UpdateSelectionCursor(a);
    CollectionBook_RefreshCompletionDigits(a);
    ForgingBook_RefreshEntryDescription(a);
    return 0;
}
