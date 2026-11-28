#include "global.h"
#include "ui/books/forging_book.h"

void ForgingBook_RemoveDisplayObjects(struct ForgingBook *book)
{
    s32 i;

    Sprite_Remove(&book->sprite);
    Task_Kill(book->attackEffectTask);
    Task_Kill(book->debrisTask);
    Anim_Remove(&book->preview);
    for (i = 0; i < 18; i++)
        Anim_Remove(&book->cells[i]);
    for (i = 0; i < 4; i++)
        Anim_Remove(&book->countDigits[i]);
    Anim_Remove(&book->categoryHeading);
    Anim_Remove(&book->completionBadge);
    Anim_Remove(&book->cursor);
    Anim_Remove(&book->background);
}
