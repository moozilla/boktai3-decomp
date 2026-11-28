#include "global.h"
#include "ui/books/forging_book.h"

s32 SceneKeyframe_UpdateSelectionVisibility(struct ForgingBook *book)
{
    Anim_LoadKeyframe(&book->categoryHeading, &book->resource, (u16)(book->category + 0x4c), 0);
    if (ForgingBook_IsCategoryComplete(book, book->category))
        book->completionBadge.flags &= ~1;
    else
        book->completionBadge.flags |= 1;
    return 0;
}
