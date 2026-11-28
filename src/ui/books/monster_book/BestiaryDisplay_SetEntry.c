#include "global.h"
#include "ui/books/bestiary_display.h"

void BestiaryDisplay_SetEntry(struct BestiaryDisplay *p, u32 entry)
{
    p->entry = entry;
    p->entryChanged = 1;
}
