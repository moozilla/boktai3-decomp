#include "global.h"
#include "ui/books/forging_book.h"
void ForgingBook_UpdateSelectionCursor(struct ForgingBook *);
void ForgingBook_UpdateCategoryCountDigits(struct ForgingBook *);
void ForgingBook_RefreshSelectedEntryText(struct ForgingBook *);

u32 ForgingBook_RefreshSelection(struct ForgingBook *book)
{
    ForgingBook_UpdateSelectionCursor(book);
    ForgingBook_UpdateCategoryCountDigits(book);
    ForgingBook_RefreshSelectedEntryText(book);
    return 0;
}
