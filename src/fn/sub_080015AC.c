#include "global.h"
#include "background_controller.h"

s32 Background_ApplyQueuedCommands(struct Background *p)
{
    u32 i;
    for (i = 0; i < p->queue.count; i++) {
        u32 type = p->queue.types[i];
        s16 *a = p->queue.commands[i].args;
        switch (type) {
            case 0:
                p->moveActive = 0;
                p->moveTime = 0;
                break;
            case 1:
                p->moveActive = 1;
                p->moveDuration = a[0];
                p->moveTime = 0;
                p->moveTargetX = a[1] << 7;
                p->moveTargetY = a[2] << 7;
                p->moveOriginX = p->x;
                p->moveOriginY = p->y;
                break;
            case 2:
                p->fadeActive = 0;
                p->blendX = a[0] << 8;
                p->blendY = a[1] << 8;
                p->blendStepX = 0;
                p->blendStepY = 0;
                sub_08219574(p->layer, p->blendX >> 8, p->blendY >> 8);
                break;
            case 3:
                p->fadeActive = 1;
                p->blendStartX = p->blendX;
                p->blendStartY = p->blendY;
                p->fadeDuration = a[0];
                p->blendTargetX = a[1] << 8;
                p->blendTargetY = a[2] << 8;
                p->blendStepX = Div(p->blendTargetX - p->blendStartX, p->fadeDuration);
                p->blendStepY = Div(p->blendTargetY - p->blendStartY, p->fadeDuration);
                sub_08219574(p->layer, p->blendX >> 8, p->blendY >> 8);
                break;
            case 6:
                p->driftX = a[0];
                p->driftY = a[1];
                break;
            case 7:
                p->scrollTime = 0;
                p->scrollDuration = a[0];
                p->scrollStartX = p->scrollSpeedX << 7;
                p->scrollStartY = p->scrollSpeedY << 7;
                p->scrollTargetX = a[1] << 7;
                p->scrollTargetY = a[2] << 7;
                if (!p->scrollDuration) {
                    p->scrollSpeedX = a[1];
                    p->scrollSpeedY = a[2];
                }
                break;
        }
    }
    return p->queue.count = 0;
}
