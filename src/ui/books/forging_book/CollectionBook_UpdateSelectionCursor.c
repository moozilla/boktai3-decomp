#include "global.h"
#include "ui/books/collection_book.h"

s32 CollectionBook_UpdateSelectionCursor(struct CollectionBook *book)
{
    book->cursor.position.x = book->column * 40;
    book->cursor.position.y = book->row * 40;
    Anim_LoadKeyframe(&book->cursor, &book->resource, 0x58, 0);
    return 0;
}
