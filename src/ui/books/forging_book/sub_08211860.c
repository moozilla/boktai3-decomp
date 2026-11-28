#include "global.h"
#include "ui/books/forging_book.h"

void sub_08211860(s32 bit)
{
    if (bit <= 31)
        gGameState->flags720 |= 1 << bit;
    else
        gGameState->flags724 |= 1 << (bit - 32);
}
