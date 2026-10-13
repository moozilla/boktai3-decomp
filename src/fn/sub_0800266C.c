#include "global.h"
#include "background_controller.h"

s32 ScrollingBackground_Initialize(struct ScrollingBackground *p, u16 id,
                                   struct ScrollingBackgroundConfig *c)
{
    struct BackgroundLayer *layer;
    u8 *resource;
    s32 x, y, scale, tx, ty;
    void *map;
    gUnk_02000010[c->layer] = p;
    p->id = id;
    p->flags = c->flags;
    p->layer = c->layer;
    layer = &gUnk_03004C30[p->layer];
    p->resource = c->resource;
    resource = sub_08216E54(p->resource);
    if (p->flags & 1) {
        struct BackgroundDimensions *dims = sub_0821A2C4(0x56C2)->data;
        c->offsetX = dims->x;
        c->offsetY = dims->y;
    }
    if (p->flags & 2) {
        x = gUnk_03005418[0] + gUnk_030042A0.offsetX;
        y = gUnk_03005418[1] + gUnk_030042A0.offsetY;
        scale = c->scale;
    } else {
        x = c->x;
        y = c->y;
        scale = c->scale;
    }
    p->x = x << 4;
    p->y = y << 4;
    p->baseX = p->x;
    p->baseY = p->y;
    p->moveTargetX = p->x;
    p->moveTargetY = p->y;
    p->speedX = c->speedX;
    p->speedY = c->speedY;
    p->accumulatedX = 0;
    p->accumulatedY = 0;
    p->offsetX = c->offsetX;
    p->offsetY = c->offsetY;
    p->width = c->width << 3;
    p->height = c->height << 3;
    p->moveActive = 0;
    p->moveDuration = 0;
    p->moveTime = 0;
    p->enabled = 1;
    map = Video_GetBackgroundMap(p->layer);
    if (resource[0] == 'M' && resource[1] == 'P') {
        layer->flags = 1;
        sub_08215A74(p->layer, c->mode);
    } else
        sub_082160B8(p->layer, 1, resource, 0, (s16)c->offsetX, (s16)c->offsetY, c->mode, map);
    p->speedScale = scale;
    {
        s32 drawX = p->x >> 4, drawY = p->y >> 4;
        tx = Div(drawX - 128, 8);
        ty = Div(drawY - 128, 8) & ~1;
        sub_0821656C(p->layer, tx, ty, drawX, drawY);
    }
    p->fadeActive = 0;
    {
        s32 blendX = c->blendX, blendY;
        p->blendX = blendX << 8;
        blendY = c->blendY;
        p->blendY = blendY << 8;
        p->blendStepX = 0;
        p->blendStepY = 0;
        p->blendTargetX = p->blendX;
        p->blendTargetY = p->blendY;
        p->blendStepX = p->blendX;
        p->blendStepY = p->blendY;
        if (c->fade)
            sub_08219574(p->layer, blendX, blendY);
    }
    return 0;
}
