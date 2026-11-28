#include "global.h"
#include "ui/books/forging_book.h"
s32 Div(s32, s32);
s32 Mod(s32, s32);

u32 ForgingBook_UpdateCategoryCountDigits(struct ForgingBook *book)
{
    s32 discovered = ForgingBook_CountDiscoveredCategoryEntries(book, book->category);
    s32 total = ForgingBook_GetCategoryEntryCount(book->category);
    s32 discoveredTens, discoveredOnes, totalTens, totalOnes;

    if (book->category > 5) {
        discovered--;
        total--;
    }
    discoveredTens = Div(discovered, 10);
    discoveredOnes = Mod(discovered, 10);
    totalTens = Div(total, 10);
    totalOnes = Mod(total, 10);
    Anim_LoadKeyframe(&book->countDigits[0], &book->resource, (u16)discoveredTens, 0);
    Anim_LoadKeyframe(&book->countDigits[1], &book->resource, (u16)discoveredOnes, 0);
    Anim_LoadKeyframe(&book->countDigits[2], &book->resource, (u16)totalTens, 0);
    Anim_LoadKeyframe(&book->countDigits[3], &book->resource, (u16)totalOnes, 0);
    return 0;
}
