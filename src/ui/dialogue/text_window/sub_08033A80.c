#include "global.h"
/* TextWindow_GetBounds: Returns the embedded text bounds as left top width and height */
#define TextWindow_GetBounds sub_08033A80

u8 *TextWindow_Find(u32);

s32 sub_08033A80(u32 id, u32 *out)
{
    u8 *w = TextWindow_Find(id);
    u8 *q;
    if (w == NULL)
        return -1;
    q = w + 0x60;
    out[0] = q[2];
    out[1] = q[3];
    out[2] = q[4];
    out[3] = q[5];
    return 0;
}
