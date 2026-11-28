#include "global.h"
#include "ui/books/forging_book.h"

s32 ForgingBook_UpdateSelectionCursor(struct ForgingBook *book)
{
    book->cursor.position.x = book->column * 24 + 0x58;
    book->cursor.position.y = book->row * 24 + 0x18;
    Anim_LoadKeyframe(&book->cursor, &book->cursorResource, 0x4d, 0);
    return 0;
}
