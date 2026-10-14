// MATCHING_CONCESSION: register-pinning
#include "background_flash.h"
#define BackgroundFlash_UpdateDisappearing sub_080479B0
#define BackgroundFlash_TakeEntryFlag      sub_08047700
#define BackgroundFlash_SetState           sub_080476F4
u32 BackgroundFlash_TakeEntryFlag(struct BackgroundFlashController *);
void BackgroundFlash_SetState(struct BackgroundFlashController *, u32);
void BackgroundFlash_UpdateDisappearing(struct BackgroundFlashController *controller)
{
    u32 visible;
    register u32 flags asm("r2");
    u32 previous;
    u32 frame;
    BackgroundFlash_TakeEntryFlag(controller);
    visible = 0x200;
    previous = gUnk_03004BD8;
    flags = visible;
    flags |= previous;
    gUnk_03004BD8 = flags;
    frame = controller->stateFrame;
    if (frame <= 9) {
        if (!(frame & 1))
            gUnk_03004BD8 = flags & ~visible;
    } else {
        BackgroundFlash_SetState(controller, 0);
        gUnk_03004BD8 &= ~visible;
    }
}
