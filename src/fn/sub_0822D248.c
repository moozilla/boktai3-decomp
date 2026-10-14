#include "global.h"
s32 sub_0822D210(void);
s32 sub_0822D248(s32 kind)
{
    switch (kind) {
        case 1:
        case 13:
        case 18:
        case 19:
        case 20:
        case 22:
        case 25:
            return 2;
        case 4:
            return 5;
        case 23:
        case 24:
            return 14;
        case 10:
        case 11:
            return sub_0822D210();
        default:
            return -1;
    }
}
