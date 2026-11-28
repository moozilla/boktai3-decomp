#include "global.h"
#include "ui/books/collection_book.h"
void ScrollingDialogue_RequestHide(void);

void ForgingBook_ReleaseGraphics(struct CollectionBook *book)
{
    s32 i;

    for (i = 0; i < 10; i++)
        Sprite_Remove(&book->entries[i]);
    Anim_Remove(&book->trackedMarker);
    for (i = 0; i < 4; i++)
        Anim_Remove(&book->countDigits[i]);
    Anim_Remove(&book->pageHeading);
    Anim_Remove(&book->completionBadge);
    Anim_Remove(&book->cursor);
    Anim_Remove(&book->background);
    ScrollingDialogue_RequestHide();
}
