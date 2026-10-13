#include "global.h"
#define SceneObjects_HasSecondaryId sub_0801E374
int SceneObjects_HasSecondaryId(u8 *p, u16 id)
{
    int i = 0;
    int initial = p[0x1d];
    if (i < initial) {
        int count = initial;
        u16 *ids = (u16 *)(p + 0x26);
        do {
            if (*ids == id)
                return 1;
            ids++;
            i++;
        } while (i < count);
    }
    return 0;
}
