#include "global.h"
#include "ui/books/bestiary_palette_variant.h"
// Family and descriptive names are tentative interpretations of static code.
void PaletteTransition_SetAlternateVariantParameters(struct BestiaryPaletteVariant *p)
{
    switch (p->variant) {
        case 62:
        case 100:
            p->counter = 19;
            break;
        case 101:
            p->counter = 139;
            break;
        case 63:
        case 102:
        case 103:
            p->counter = 199;
            break;
        default:
            return;
    }
    p->enabled = 3;
}
