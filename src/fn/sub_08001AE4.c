#include "global.h"
// MATCHING_CONCESSION: register-pinning
#include "background_controller.h"

static inline u8 modeFlag(struct Background *p)
{
    return p->flags & 16;
}

s32 Background_Initialize(struct Background *p, u16 id, struct BackgroundConfig *c)
{
    struct BackgroundLayer *layer;
    u8 *resource;
    register s32 offsetX asm("r9"), offsetY asm("r10");
    s32 x, y;
    u32 mode;
    gUnk_02000000[c->layer] = p;
    p->id = id;
    p->flags = c->flags;
    p->layer = c->layer;
    layer = &gUnk_03004C30[p->layer];
    p->resource = c->resource;
    resource = sub_08216E54(p->resource);
    if (p->flags & 1) {
        struct BackgroundDimensions *dims = sub_0821A2C4(0x56C2)->data;
        offsetX = dims->x;
        offsetY = dims->y;
    } else {
        offsetX = 0;
        offsetY = 0;
    }
    if (p->flags & 2) {
        x = gUnk_03005418[0] + gUnk_030042A0.offsetX;
        y = gUnk_03005418[1] + gUnk_030042A0.offsetY;
    } else {
        x = c->x;
        y = c->y;
    }
    p->rateX = c->rateX;
    p->rateY = c->rateY;
    layer->x = x - 120;
    layer->y = y - 80;
    p->x = x << 7;
    p->y = y << 7;
    p->baseX = p->x;
    p->baseY = p->y;
    p->moveTargetX = p->x;
    p->moveTargetY = p->y;
    p->scrollSpeedX = c->scrollX;
    p->scrollSpeedY = c->scrollY;
    p->scrollDistX = 0;
    p->scrollDistY = 0;
    p->cameraOffsetX = offsetX >> 7;
    p->cameraOffsetY = offsetY >> 7;
    p->driftX = c->offsetX;
    p->driftY = c->offsetY;
    p->limitX = c->limitX;
    p->limitY = c->limitY;
    p->centerX = c->centerX;
    p->centerY = c->centerY;
    p->moveActive = 0;
    p->moveDuration = 0;
    p->moveTime = 0;
    p->fadeActive = 0;
    p->blendX = c->blendX << 8;
    p->blendY = c->blendY << 8;
    p->blendStepX = 0;
    p->blendStepY = 0;
    p->blendTargetX = p->blendX;
    p->blendTargetY = p->blendY;
    p->blendStepX = p->blendX;
    p->blendStepY = p->blendY;
    mode = modeFlag(p) != 0;
    if (resource[0] == 'M' && resource[1] == 'P') {
        layer->flags = mode;
        sub_08215A74(p->layer, c->mode);
    } else {
        void *map = Video_GetBackgroundMap(p->layer);
        u32 layerId = p->layer;
        register u32 drawMode asm("r1") = mode;
        sub_082160B8(layerId, drawMode, resource, 0, offsetX, offsetY, c->mode, map);
    }
    if (p->limitX && p->limitY) {
        layer->halfWidth = p->limitX / 2;
        layer->halfHeight = p->limitY / 2;
    }
    return 0;
}
