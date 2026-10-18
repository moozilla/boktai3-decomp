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
void PaletteTransition_SetAlternateVariantParameters(struct PaletteTransition *p)
{
    switch (p->variant) {
        case 62:
        case 100:
            p->parameter9fc = 19;
            break;
        case 101:
            p->parameter9fc = 139;
            break;
        case 63:
        case 102:
        case 103:
            p->parameter9fc = 199;
            break;
        default:
            return;
    }
    p->enabled = 3;
}
