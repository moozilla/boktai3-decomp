#include "global.h"
extern u8 *gUnk_020000B8[8];
u8 *SceneActors_FindAvailable(void)
{
    u8 *found = 0;
    u8 **cursor;
    int i = 0;
    cursor = gUnk_020000B8;
    do {
        u8 *p;
        p = *cursor;
        if (p && !p[0xb3]) {
            if (found) {
                break;
            }
            found = p;
        }
        cursor++;
        i++;
    } while (i < 8);
    do {
        do {
            return found;
        } while (0);
    } while (0);
}
