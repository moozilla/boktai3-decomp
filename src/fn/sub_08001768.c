#include "global.h"
// MATCHING_CONCESSION: register-pinning
/* Empty constraints retain register choices without handwritten instructions. */
#include "background_controller.h"

#define truncate7(v)                                                                               \
    ({                                                                                             \
        s32 value = (v);                                                                           \
        s32 result;                                                                                \
        if (value >= 0)                                                                            \
            result = value >> 7;                                                                   \
        else                                                                                       \
            result = -((-value) >> 7);                                                             \
        result;                                                                                    \
    })
#define truncate3(v)                                                                               \
    ({                                                                                             \
        s32 value = (v);                                                                           \
        s32 result;                                                                                \
        if (value >= 0)                                                                            \
            result = value >> 3;                                                                   \
        else                                                                                       \
            result = -((-value) >> 3);                                                             \
        result;                                                                                    \
    })

s32 Background_Update(struct Background *p)
{
    s32 x, y asm("r6");
    s32 tileX, tileY;

    Background_ApplyQueuedCommands(p);
    tileY = 0;
    if (p->flags & 2) {
        s32 cx, cy;
        register s32 bottom asm("r5");
        if (!gUnk_030042A0.mode) {
            cx = gUnk_03005418[0] + gUnk_030042A0.offsetX;
            cy = gUnk_03005418[1] + gUnk_030042A0.offsetY;
        } else {
            cx = gUnk_030042A0.x >> 3;
            cy = gUnk_030042A0.y >> 3;
        }
        if (!(p->flags & 1)) {
            cx -= p->cameraOffsetX;
            cy -= p->cameraOffsetY;
        }
        if (p->flags & 128) {
            cx += p->driftX;
            cy += p->driftY;
        }
        {
            s32 cyHint = cy;
            cy = cyHint;
        }
        if (p->rateX == 128)
            x = cx;
        else
            x = ({
                s32 delta = cx - p->centerX;
                register s32 result asm("r0") = truncate7(delta * p->rateX) + p->centerX;
                asm("" : "+r"(result));
                result;
            });
        x -= 120;
        if (p->rateY == 128)
            y = cy;
        else
            y = ({
                s32 delta = cy - p->centerY;
                register s32 result asm("r0") = truncate7(delta * p->rateY) + p->centerY;
                asm("" : "+r"(result));
                result;
            });
        y -= 80;
        bottom = y + 80;
        tileX = truncate3(x - 8);
        {
            s32 value = truncate3(bottom - 128);
            register s32 mask asm("r8") = -2;
            tileY = mask & value;
        }
    } else {
        if (p->flags & 4) {
            if (p->scrollDuration) {
                s32 remaining = p->scrollDuration - p->scrollTime;
                asm("" : "+r"(remaining));
                p->scrollSpeedX =
                    Div(p->scrollStartX * remaining + p->scrollTargetX * p->scrollTime,
                        p->scrollDuration) /
                    128;
                p->scrollSpeedY =
                    Div(p->scrollStartY * remaining + p->scrollTargetY * p->scrollTime,
                        p->scrollDuration) /
                    128;
                if (++p->scrollTime > p->scrollDuration) {
                    p->scrollDuration = 0;
                    p->scrollTime = 0;
                    p->scrollSpeedX = p->scrollTargetX / 128;
                    p->scrollSpeedY = p->scrollTargetY / 128;
                }
            }
            p->scrollDistX += p->scrollSpeedX;
            p->scrollDistY += p->scrollSpeedY;
            p->x = p->scrollDistX + p->baseX;
            p->y = p->scrollDistY + p->baseY;
        } else if (p->moveActive == 1) {
            if (p->moveDuration) {
                register s32 remaining asm("r5") = p->moveDuration - p->moveTime;
                p->x = Div(p->moveOriginX * remaining + p->moveTargetX * p->moveTime,
                           p->moveDuration);
                p->y = Div(p->moveOriginY * remaining + p->moveTargetY * p->moveTime,
                           p->moveDuration);
            }
            if (++p->moveTime >= p->moveDuration) {
                p->moveActive = 0;
                p->moveTime = 0;
                p->x = p->moveTargetX;
                p->y = p->moveTargetY;
            }
        }
        x = ({
            register s32 value asm("r3") = truncate7(p->x);
            asm("" : "+r"(value));
            value;
        });
        y = ({
            s32 value = truncate7(p->y);
            value;
        });
        if (p->flags & 32) {
            x += gUnk_03005418[0] + gUnk_030042A0.offsetX;
            y += gUnk_03005418[1] + gUnk_030042A0.offsetY;
        }
        {
            register s32 rowY asm("r5") = y;
            if (x < 0)
                tileX = ~((-x) >> 3);
            else
                tileX = x >> 3;
            if (rowY < 0) {
                register s32 negative asm("r0") = ~((-rowY) >> 3);
                asm("" : "+r"(negative));
                tileY = negative;
            } else
                tileY = rowY >> 3;
            tileY &= ~1;
        }
    }
    sub_0821656C(p->layer, tileX, tileY, x, y);
    if (p->fadeActive == 1) {
        p->blendX += p->blendStepX;
        p->blendY += p->blendStepY;
        sub_08219574(p->layer, p->blendX >> 8, p->blendY >> 8);
        if (--p->fadeDuration == 0)
            p->fadeActive = 0;
    }
    if (p->flags & 64) {
        if (p->frame & 1) {
            u32 mask = (0x01000000u << p->layer) >> 16;
            gUnk_03004BD8 = mask | gUnk_03004BD8;
        } else {
            u32 mask = (0x01000000u << p->layer) >> 16;
            gUnk_03004BD8 &= ~mask;
        }
    }
    p->frame++;
    return 0;
}
