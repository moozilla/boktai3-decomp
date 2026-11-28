#include "global.h"
#include "ui/books/collection_book.h"

void ForgingBook_UpdateCursorPulse(struct CollectionBook *book)
{
    s16 frame = book->pulseFrame;
    u32 color;

    if (frame <= 9)
        color = 0x1f;
    else if (frame <= 0x11)
        color = 0x1b;
    else if (frame <= 0x19)
        color = 0x12;
    else if (frame <= 0x21)
        color = 10;
    else if (frame <= 0x29)
        color = 0x12;
    else
        color = 0x1b;
    book->pulseFrame++;
    if (book->pulseFrame > 0x31)
        book->pulseFrame = 0;
    book->cursorColors[12] = color;
}
