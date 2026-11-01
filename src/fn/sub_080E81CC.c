#include "global.h"
/* Every quadrant has a case, but the switch keeps a default path that falls off the end with no
 * return statement; r0 still holds direction there, as in the ROM. */
u32 SceneActor_GetDirectionFlags(s32 direction)
{
    switch (((direction + 16) & 255) >> 6) {
        case 0:
        case 2:
            return 0;
        case 1:
        case 3:
            return 1;
    }
}
