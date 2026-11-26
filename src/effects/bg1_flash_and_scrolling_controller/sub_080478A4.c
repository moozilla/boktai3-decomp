#include "effects/bg1_flash_and_scrolling_controller/background_flash.h"
u32 BackgroundFlash_TakeEntryFlag(struct BackgroundFlashController *);
void BackgroundFlash_SetState(struct BackgroundFlashController *, u32);
void BackgroundFlash_UpdateAppearing(struct BackgroundFlashController *controller)
{
    u32 visible;
    u32 flags;
    u16 *control;
    u32 frame;
    if (BackgroundFlash_TakeEntryFlag(controller)) {
        BackgroundFlash_ShowLayer();
        controller->page = 0;
    }
    visible = 0x200;
    control = &gDispCntShadow;
    flags = *control | 0x200;
    *control = flags;
    frame = controller->stateFrame;
    if (frame <= 14) {
        if ((frame & 3) <= 2) {
            flags &= ~visible;
            *control = flags;
        }
    } else if (frame <= 29) {
        if ((frame & 3) <= 1) {
            flags &= ~visible;
            *control = flags;
        }
    } else if (!(frame & 3)) {
        flags &= ~visible;
        *control = flags;
    }
    if (frame > 44) {
        BackgroundFlash_SetState(controller, 2);
    }
}
