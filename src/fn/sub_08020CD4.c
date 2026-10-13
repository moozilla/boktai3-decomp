#include "global.h"
extern void *gUnk_0200047C;
int sub_08020990(void *, void *);
int SceneMessages_RegisterReceiver(u8 *p, int id, int kind)
{
    int result;
    if (gUnk_0200047C) {
        int zero = 0;
        *(u16 *)p = id;
        p[2] = kind;
        p[6] = zero;
        p[7] = zero;
        p[3] = zero;
        result = sub_08020990(gUnk_0200047C, p);
    } else
        result = -1;
    return result;
}
