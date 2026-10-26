#include "background_flash.h"
u32 BackgroundFlash_TakeEntryFlag(struct BackgroundFlashController *);
void BackgroundFlash_SetState(struct BackgroundFlashController *, u32);
void BackgroundFlash_UpdateDisappearing(struct BackgroundFlashController *controller)
{
    u32 visible;
    u32 flags;
    u32 previous;
    u32 frame;
    do {
        BackgroundFlash_TakeEntryFlag(controller);
        visible = 0x200;
        previous = gDispCntShadow;
    } while (0);
    flags = visible;
    flags |= previous;
    do {
        gDispCntShadow = flags;
        frame = controller->stateFrame;
    } while (0);
    if (frame <= 9) {
        if (!(frame & 1))
            gDispCntShadow = flags & ~visible;
    } else {
        BackgroundFlash_SetState(controller, 0);
        gDispCntShadow &= ~visible;
    }
}
