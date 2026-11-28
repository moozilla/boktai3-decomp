#include "global.h"
#include "ui/books/bestiary_display.h"

void BestiaryDisplay_SetPosition(struct BestiaryDisplay *p, u16 x, u16 y)
{
    p->origin[0] = x;
    p->origin[1] = y;
    p->origin[2] = 0;
}
