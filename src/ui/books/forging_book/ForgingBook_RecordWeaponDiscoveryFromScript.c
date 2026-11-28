#include "global.h"
#include "ui/books/forging_book.h"

void ForgingBook_RecordWeaponDiscoveryFromScript(void)
{
    if (Script_SeekToKeyword('i')) {
        u32 index = Script_GetValue();

        if (index <= 31)
            gGameState->flags72C |= 1 << index;
        else
            gGameState->flags730 |= 1 << (index - 32);
    }
}
