#include "global.h"
#include "color_fade_controller.h"

s32 sub_08003860(struct ColorFadeChannel *c, s32 x)
{
    u32 *p = &c->mode;
    switch (x) {
        case 1:
            if (*p > 2)
                return -1;
            *p = 1;
            break;
        case 2:
            if (*p > 2)
                return -1;
            *p = 2;
            break;
        case 3:
            *p = 3;
            break;
        case 4:
        case 5:
            *p = 4;
            break;
    }
    return 0;
}
