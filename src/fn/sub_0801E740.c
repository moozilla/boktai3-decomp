#include "global.h"
#define SceneObjects_Unlock sub_0801E740
void sub_0801EF58(void);
void SceneObjects_Unlock(u8 *p, u16 id)
{
    int i;
    for (i = 0; i < *(u16 *)(p + 0x1a); i++) {
        u8 *s = *(u8 **)(p + 0x42c) + 140 * i;
        if (*(u16 *)(s + 0x34) == id)
            s[0x36] &= ~1;
    }
}
