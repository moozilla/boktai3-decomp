#include "background_flash.h"
#define BackgroundFlash_Update          sub_080479F4
#define BackgroundFlash_UpdateScrolling sub_08047770
void BackgroundFlash_UpdateScrolling(struct BackgroundFlashController *);
extern void (*const gUnk_086059DC[])(struct BackgroundFlashController *);
s32 BackgroundFlash_Update(struct BackgroundFlashController *controller)
{
    sub_0824923C(controller, (void *)gUnk_086059DC[controller->state]);
    if (controller->state)
        BackgroundFlash_UpdateScrolling(controller);
    ++controller->stateFrame;
    ++controller->frame;
    return 0;
}
