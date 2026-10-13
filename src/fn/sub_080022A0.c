#include "global.h"
#include "background_controller.h"

static inline s32 fixed4(s32 v)
{
    return v << 4;
}

s32 ScrollingBackground_ApplyCommands(struct ScrollingBackground *p)
{
    u32 i;
    for (i = 0; i < p->queue.count; i++) {
        u32 type = p->queue.types[i];
        s16 *a = p->queue.commands[i].args;
        switch (type) {
            case 0:
                p->moveActive = 0;
                p->moveTime = 0;
                p->moveDx = 0;
                p->moveDy = 0;
                break;
            case 1:
                p->moveActive = 1;
                p->moveDuration = a[0];
                p->moveTime = 0;
                p->moveTargetX = fixed4(a[1]);
                p->moveTargetY = fixed4(a[2]);
                p->moveDx = Div(p->moveTargetX - p->x, p->moveDuration);
                p->moveDy = Div(p->moveTargetY - p->y, p->moveDuration);
                break;
            case 2:
                sub_08219574(p->layer, a[0], a[1]);
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
            case 4:
                p->enabled = 0;
                break;
            case 5:
                p->enabled = 1;
                break;
        }
    }
    return 0;
}
