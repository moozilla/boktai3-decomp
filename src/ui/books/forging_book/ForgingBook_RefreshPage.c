#include "global.h"
#include "ui/books/collection_book.h"
void ForgingBook_SetupPageBackground(struct CollectionBook *);
void CollectionBook_RefreshCollectedEntryVisibility(struct CollectionBook *);
void CollectionBook_UpdateSelectionCursor(struct CollectionBook *);
void CollectionBook_RefreshCompletionDigits(struct CollectionBook *);
void CollectionBook_RefreshPageHeading(struct CollectionBook *);
void CollectionBook_UpdateTrackedEntryMarker(struct CollectionBook *);
void ForgingBook_RefreshEntryDescription(struct CollectionBook *);

u32 ForgingBook_RefreshPage(struct CollectionBook *book)
{
    ForgingBook_SetupPageBackground(book);
    CollectionBook_RefreshCollectedEntryVisibility(book);
    CollectionBook_UpdateSelectionCursor(book);
    CollectionBook_RefreshCompletionDigits(book);
    CollectionBook_RefreshPageHeading(book);
    CollectionBook_UpdateTrackedEntryMarker(book);
    ForgingBook_RefreshEntryDescription(book);
    return 0;
}
