#include "global.h"
#include "ui/books/collection_book.h"
void CollectionBook_UpdateSelectionCursor(struct CollectionBook *);
void CollectionBook_RefreshCompletionDigits(struct CollectionBook *);
void ForgingBook_RefreshEntryDescription(struct CollectionBook *);

u32 sub_0821353C(struct CollectionBook *book)
{
    CollectionBook_UpdateSelectionCursor(book);
    CollectionBook_RefreshCompletionDigits(book);
    ForgingBook_RefreshEntryDescription(book);
    return 0;
}
