#include "global.h"
void ForgingBook_SetupPageBackground(u32);
void sub_0821304C(u32);
void CollectionBook_UpdateSelectionCursor(u32);
void CollectionBook_RefreshCompletionDigits(u32);
void CollectionBook_RefreshPageHeading(u32);
void CollectionBook_UpdateTrackedEntryMarker(u32);
void ForgingBook_RefreshEntryDescription(u32);
u32 ForgingBook_RefreshPage(u32 a)
{
    ForgingBook_SetupPageBackground(a);
    sub_0821304C(a);
    CollectionBook_UpdateSelectionCursor(a);
    CollectionBook_RefreshCompletionDigits(a);
    CollectionBook_RefreshPageHeading(a);
    CollectionBook_UpdateTrackedEntryMarker(a);
    ForgingBook_RefreshEntryDescription(a);
    return 0;
}
