#include "global.h"
// Family and descriptive names are tentative interpretations of static code.
struct PaletteTransition {
    u8 pad0[0x1c];
    u32 variant;
    u8 pad20[0x9dc];
    u16 parameter9fc;
    u8 pad9fe[2];
    u16 enabled;
};
void PaletteTransition_SetVariantParameters(struct PaletteTransition *p)
{
    switch (p->variant) {
        case 100:
            p->parameter9fc = 2;
            break;
        case 101:
            p->parameter9fc = 122;
            break;
        case 102:
        case 103:
            p->parameter9fc = 182;
            break;
        default:
            return;
    }
    p->enabled = 1;
}
