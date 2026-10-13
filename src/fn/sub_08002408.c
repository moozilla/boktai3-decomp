#include "global.h"
#include "background_controller.h"

#define fixedToInt(v)                                                                              \
    ({                                                                                             \
        s32 value = (v);                                                                           \
        s32 result;                                                                                \
        if (value >= 0)                                                                            \
            result = value >> 18;                                                                  \
        else                                                                                       \
            result = -((-value) >> 18);                                                            \
        result;                                                                                    \
    })

static inline s32 pixel(s16 v)
{
    if (v >= 0)
        return v >> 4;
    return -((-v) >> 4);
}

s32 ScrollingBackground_Update(struct ScrollingBackground *p)
{
    struct BackgroundLayer *layer = &gUnk_03004C30[p->layer];
    s32 width, height;
    s32 cx, cy;
    ScrollingBackground_ApplyCommands(p);
    if (!p->enabled)
        return 0;
    width = p->width << 4;
    height = p->height << 4;
    if (p->flags & 8) {
        s32 speed = gUnk_02000550 * (s16)p->speedScale;
        s32 angle = (gUnk_0200053C + 160) & 255;
        s32 x, y;
        const s16 *sine = gUnk_086149C4;
        p->accumulatedX += sine[(angle + 64) & 255] * speed;
        p->accumulatedY += sine[angle & 255] * speed;
        x = fixedToInt(p->accumulatedX);
        y = fixedToInt(p->accumulatedY);
        if (x > width)
            x -= width;
        else if (x < -width)
            x += width;
        if (y > height)
            y -= height;
        else if (y < -height)
            y += height;
        p->x = p->baseX + x;
        p->y = p->baseY + y;
        layer->x = p->x;
        layer->y = p->y;
    } else {
        p->accumulatedX += p->speedX;
        if (p->accumulatedX > width)
            p->accumulatedX -= width;
        else if (p->accumulatedX < -width)
            p->accumulatedX += width;
        p->accumulatedY += p->speedY;
        if (p->accumulatedY > height)
            p->accumulatedY -= height;
        else if (p->accumulatedY < -height)
            p->accumulatedY += height;
        p->x = p->accumulatedX + p->baseX;
        p->y = p->accumulatedY + p->baseY;
        layer->x = pixel(p->x);
        layer->y = pixel(p->y);
    }
    cx = gUnk_03005418[0] + gUnk_030042A0.offsetX;
    cy = gUnk_03005418[1] + gUnk_030042A0.offsetY;
    if (p->width)
        layer->x += Mod(cx - 120, p->width);
    else {
        s32 shifted = (u16)layer->x - 120;
        layer->x = shifted + cx;
    }
    if (p->height)
        layer->y += Mod(cy - 80, p->height);
    else {
        s32 shifted = (u16)layer->y - 80;
        layer->y = shifted + cy;
    }
    if (p->fadeActive == 1) {
        p->blendX += p->blendStepX;
        p->blendY += p->blendStepY;
        sub_08219574(p->layer, p->blendX >> 8, p->blendY >> 8);
        if (--p->fadeDuration == 0)
            p->fadeActive = 0;
    }
}
