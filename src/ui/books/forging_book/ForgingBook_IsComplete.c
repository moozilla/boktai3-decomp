#include "global.h"
#include "ui/books/forging_book.h"

u32 ForgingBook_IsComplete(void)
{
    if (ForgingBook_HasDiscoveredAllWeapons() && ForgingBook_HasDiscoveredAllItems())
        return 1;
    return 0;
}
