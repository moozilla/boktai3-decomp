#include "background_flash.h"
u32 BackgroundFlash_TakeEntryFlag(struct BackgroundFlashController *);
void BackgroundFlash_SetState(struct BackgroundFlashController *, u32);
void BackgroundFlash_UpdateAppearing(struct BackgroundFlashController *controller)
{
    u32 visible;
    u32 flags;
    u32 previous;
    u16 *control;
    u32 frame;
    if (BackgroundFlash_TakeEntryFlag(controller)) {
        BackgroundFlash_ShowLayer();
        controller->page = 0;
    }
    visible = 0x200;
    control = &gDispCntShadow;
    previous = *control;
    do {
        flags = visible;
        flags |= previous;
        *control = flags;
    } while (0);
    frame = controller->stateFrame;
    if (frame > 14) {
        goto middle;
    }
    if ((frame & 3) > 2) {
        goto checked;
    }
    goto hidden;
middle:
    if (frame > 29) {
        goto last;
    }

    if ((frame & 3) > 1) {
        goto checked;
    }
    goto hidden;
last:
    if (frame & 3) {
        goto checked;
    }

hidden:
    flags &= ~visible;
    *control = flags;
checked:
    if (frame > 44) {
        BackgroundFlash_SetState(controller, 2);
    }
}
