#include "global.h"
u8 *ResourceRegistry_GetResource(int);
int SceneObjects_HasTypeTwoResource(int id)
{
    u8 *p = ResourceRegistry_GetResource(0x85e6);
    int i;
    if (p)
        goto scan;
    goto fail;
success:
    return 1;
scan:
    for (i = 0; i < *(u16 *)(p + 0x1a); i++) {
        u8 *s = *(u8 **)(p + 0x42c) + i * 140;
        if (s[0x40] == 2 && *(u16 *)(s + 0x3c) == id)
            goto success;
    }
fail:
    return 0;
}
