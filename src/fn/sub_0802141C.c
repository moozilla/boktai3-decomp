#include "global.h"
#include "ram.h"
u8 *SceneActors_FindAvailable(void)
{
    u8 *found = 0;
    u8 **cursor;
    int i = 0;
    cursor = gSceneActorRegistry;
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
