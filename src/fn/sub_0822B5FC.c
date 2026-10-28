#include "global.h"
#include "scene_stimulus.h"
u8 Terrain_IsPositionInBox(u16, void *);
u8 sub_0822B5FC(void *point)
{
    struct SceneStimulusState *ctx = gSceneStimulusState;
    s32 i;
    s32 *p;
    if (ctx != NULL)
        goto start;
    goto fail;
found:
    return 1;
start:
    i = 0;
    p = ctx->ids;
    do {
        if (*p > 0 && Terrain_IsPositionInBox((u16)*p, point))
            goto found;
        p++;
        i++;
    } while (i <= 15);
fail:
    return 0;
}
