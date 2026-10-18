#include "global.h"
s32 StatGauge_GetSelectionGroup(s32 selection)
{
    s32 group;
    switch (selection >> 2) {
        case 2:
            group = 2;
            break;
        case 1:
        case 3:
            group = 1;
            break;
        case 0:
        default:
            group = 0;
            break;
    }
    return group;
}
