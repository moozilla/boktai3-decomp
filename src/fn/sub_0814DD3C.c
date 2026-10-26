#include "global.h"
#include "engine.h"

u32 Player_GetAttackAttributeFlags(u32 x)
{
    u32 r;
    if (gEngineFlags & 0x1000) {
        r = 0;
    } else {
        switch (x) {
            case 1:
                r = 0x40;
                break;
            case 2:
                r = 0x200004;
                break;
            case 3:
                r = 0x400008;
                break;
            case 4:
                r = 0x10;
                break;
            case 5:
                r = 0x20;
                break;
            case 0:
            case 7:
                r = 1;
                break;
            case 8:
                r = 2;
                break;
            default:
                r = 0;
                break;
        }
    }
    return r;
}
