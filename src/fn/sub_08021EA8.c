#include "global.h"
int SceneActorGraphics_GetResourceId(u8 *p)
{
    switch (p[0xb3]) {
        case 0:
            return 0x335;
        case 1:
            return 0x335;
        default:
            return 0x335;
    }
}
