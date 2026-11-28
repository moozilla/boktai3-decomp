#include "global.h"
#include "ui/books/forging_book.h"

u32 ForgingBook_RefreshEntrySprites(struct ForgingBook *book)
{
    s32 offset = ForgingBook_GetCategoryEntryOffset(book->category);
    s32 count = ForgingBook_GetCategoryEntryCount(book->category);
    s32 i;

    for (i = 0; i < 18; i++)
        book->cells[i].flags |= 1;
    if (book->category <= 5) {
        for (i = 0; i < count; i++) {
            if (ForgingBook_IsWeaponDiscovered(book->entries[offset + i]))
                book->cells[i].flags &= ~1;
            Anim_LoadKeyframe(&book->cells[i], &book->cellResource,
                              (u16)ForgingBook_GetEntryAnimation(book->entries[offset + i]), 0);
        }
        for (i = count; i <= 17; i++) {
            book->cells[i].flags &= ~1;
            Anim_LoadKeyframe(&book->cells[i], &book->cellResource, 0x189, 0);
        }
    } else {
        for (i = 0; i < count - 1; i++) {
            if (ForgingBook_IsItemDiscovered(book->entries[offset + i]))
                book->cells[i].flags &= ~1;
            Anim_LoadKeyframe(&book->cells[i], &book->cellResource,
                              (u16)ForgingBook_GetEntryAnimation(book->entries[offset + i] + 0x31),
                              0);
        }
        for (i = count - 1; i <= 17; i++) {
            book->cells[i].flags &= ~1;
            Anim_LoadKeyframe(&book->cells[i], &book->cellResource, 0x189, 0);
        }
    }
    return 0;
}
