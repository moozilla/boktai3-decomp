#include "global.h"
#include "ui/books/bestiary_display.h"

s32 SceneDisplay_GetAlternateDescriptor(struct BestiaryDisplay *p)
{
    s32 entry;
    switch (p->entry) {
        case 0x3e:
        case 0x64:
            entry = 0x65;
            break;
        case 0x65:
            entry = 0x64;
            break;
        case 0x3f:
        case 0x66:
            entry = 0x67;
            break;
        case 0x67:
            entry = 0x66;
            break;
        default:
            entry = 0;
            break;
    }
    return entry;
}
