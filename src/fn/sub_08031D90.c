#include "global.h"
#include "ram.h"
struct Actor {
    u8 pad[0x1A];
    u8 rect[4];
};
int MessageWindow_GetTextBounds(int *out)
{
    struct Actor *p = gMessageWindow;
    int result;
    if (!p)
        goto fail;
    {
        out[0] = p->rect[0];
        out[1] = p->rect[1];
        out[2] = p->rect[2];
        out[3] = p->rect[3];
        result = 0;
        goto done;
    }
fail:
    result = -1;
done:
    return result;
}
