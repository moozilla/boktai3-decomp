#include "global.h"
#include "ui/books/collection_book.h"

s32 CollectionBook_RefreshPageHeading(struct CollectionBook *book)
{
    Anim_LoadKeyframe(&book->pageHeading, &book->resource, (u16)(book->page + 0x59), 0);
    return 0;
}
