#include "global.h"
#include "ui/books/bestiary_palette_variant.h"
// Family and descriptive names are tentative interpretations of static code.
void PaletteTransition_SetVariantParameters(struct BestiaryPaletteVariant *p)
{
    switch (p->variant) {
        case 100:
            p->counter = 2;
            break;
        case 101:
            p->counter = 122;
            break;
        case 102:
        case 103:
            p->counter = 182;
            break;
        default:
            return;
    }
    p->enabled = 1;
}
