#include "global.h"
struct Context {
    u8 pad[0x98];
    s32 values[16];
};
extern struct Context *gUnk_030025FC;
u8 Terrain_IsPositionInBox(u16, void *);
u8 sub_0822B5FC(void *point)
{
    struct Context *ctx = gUnk_030025FC;
    s32 i;
    s32 *p;
    if (ctx != NULL)
        goto start;
    goto fail;
found:
    return 1;
start:
    i = 0;
    p = ctx->values;
    do {
        if (*p > 0 && Terrain_IsPositionInBox((u16)*p, point))
            goto found;
        p++;
        i++;
    } while (i <= 15);
fail:
    return 0;
}
