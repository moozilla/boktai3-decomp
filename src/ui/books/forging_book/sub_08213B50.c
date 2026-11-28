#include "global.h"
#include "ui/books/collection_book.h"
void ForgingBook_ReleaseGraphics(struct CollectionBook *);
u32 ForgingBook_FinalizeState(struct CollectionBook *);

u32 sub_08213B50(struct CollectionBook *book)
{
    ForgingBook_ReleaseGraphics(book);
    return ForgingBook_FinalizeState(book);
}
