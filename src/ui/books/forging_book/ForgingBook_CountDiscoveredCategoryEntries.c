#include "global.h"
#include "ui/books/forging_book.h"

s32 ForgingBook_CountDiscoveredCategoryEntries(struct ForgingBook *book, s32 category)
{
    s32 count = ForgingBook_GetCategoryEntryCount(category);
    s32 offset = ForgingBook_GetCategoryEntryOffset(category);
    s32 discovered = 0;
    s32 i;

    if (category <= 5) {
        for (i = 0; i < count; i++) {
            if (ForgingBook_IsWeaponDiscovered(book->entries[offset + i]))
                discovered++;
        }
    } else {
        for (i = 0; i < count; i++) {
            if (ForgingBook_IsItemDiscovered(book->entries[offset + i]))
                discovered++;
        }
    }
    return discovered;
}
