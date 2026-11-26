#include "effects/bg1_flash_and_scrolling_controller/background_flash.h"
u32 BackgroundFlash_TakeEntryFlag(struct BackgroundFlashController *);
void BackgroundFlash_SetState(struct BackgroundFlashController *, u32);
void BackgroundFlash_UpdateDisappearing(struct BackgroundFlashController *controller)
{
    u32 visible;
    u32 flags;
    u32 frame;
    BackgroundFlash_TakeEntryFlag(controller);
    visible = 0x200;
    flags = gDispCntShadow | 0x200;
    gDispCntShadow = flags;
    frame = controller->stateFrame;
    if (frame <= 9) {
        if (!(frame & 1))
            gDispCntShadow = flags & ~visible;
    } else {
        BackgroundFlash_SetState(controller, 0);
        gDispCntShadow &= ~visible;
    }
}
