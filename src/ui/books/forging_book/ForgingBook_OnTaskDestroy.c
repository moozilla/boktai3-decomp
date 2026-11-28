#include "global.h"
#include "ui/books/forging_book.h"
void ForgingBook_RemoveDisplayObjects(struct ForgingBook *);
void ForgingBook_ReleaseBackgrounds(struct ForgingBook *);

u32 ForgingBook_OnTaskDestroy(struct ForgingBook *book)
{
    ForgingBook_RemoveDisplayObjects(book);
    ForgingBook_ReleaseBackgrounds(book);
    gForgingBook = 0;
}
