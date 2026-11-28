#include "global.h"
#include "ui/books/forging_book.h"

void ForgingBook_RecordItemDiscoveryFromScript(void)
{
    if (Script_SeekToKeyword('i')) {
        u32 index = Script_GetValue();

        if (index <= 31)
            gGameState->flags720 |= 1 << index;
        else
            gGameState->flags724 |= 1 << (index - 32);
    }
}
