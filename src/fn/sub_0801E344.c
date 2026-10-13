#include "global.h"
#define SceneObjects_HasPrimaryId sub_0801E344
int SceneObjects_HasPrimaryId(u8 *p, u16 id)
{
    int i = 0;
    int initial = p[0x1c];
    if (i < initial) {
        int count = initial;
        u16 *ids = (u16 *)(p + 0x1e);
        do {
            if (*ids == id)
                return 1;
            ids++;
            i++;
        } while (i < count);
    }
    return 0;
}
