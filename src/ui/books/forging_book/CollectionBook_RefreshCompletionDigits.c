#include "global.h"
#include "ui/books/collection_book.h"
s32 Div(s32, s32);
s32 Mod(s32, s32);

s32 CollectionBook_RefreshCompletionDigits(struct CollectionBook *book)
{
    s32 collected = CollectionBook_CountCollectedEntries();
    s32 collectedTens = Div(collected, 10), collectedOnes = Mod(collected, 10);
    s32 totalTens = Div(10, 10), totalOnes = Mod(10, 10);

    Anim_LoadKeyframe(&book->countDigits[0], &book->resource, (u16)collectedTens, 0);
    Anim_LoadKeyframe(&book->countDigits[1], &book->resource, (u16)collectedOnes, 0);
    Anim_LoadKeyframe(&book->countDigits[2], &book->resource, (u16)totalTens, 0);
    Anim_LoadKeyframe(&book->countDigits[3], &book->resource, (u16)totalOnes, 0);
    return 0;
}
