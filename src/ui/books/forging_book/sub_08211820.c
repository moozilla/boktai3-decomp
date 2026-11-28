#include "global.h"
#include "ui/books/forging_book.h"

void sub_08211820(s32 bit)
{
    if (bit <= 31)
        gGameState->flags72C |= 1 << bit;
    else
        gGameState->flags730 |= 1 << (bit - 32);
}
