#include "global.h"

struct ForgingBook {
    u8 unknown[0xB84];
    u8 entries[1];
};
s32 ForgingBook_GetCategoryEntryCount(u32);
s32 ForgingBook_GetCategoryEntryOffset(s32);
u32 ForgingBook_IsItemDiscovered(s32);
u32 ForgingBook_IsWeaponDiscovered(s32);

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
