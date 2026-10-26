#ifndef GUARD_BACKGROUND_FLASH_H
#define GUARD_BACKGROUND_FLASH_H
#include "background_controller.h"
/* Tentative gameplay identity: BG1 flashing and dual-page scroll task, size 0x48. */
struct BackgroundFlashController {
    u8 task[0x18];
    u32 frame;
    u8 state, enter, page, unknown1F;
    void *effectSourceA, *effectSourceB;
    u32 stateFrame;
    s32 speed, targetSpeed;
    u32 retargetTimer;
    s32 xOffsets[2], yOffsets[2];
};
static inline void BackgroundFlash_ShowLayer(void)
{
    u32 visible = 0x200;
    visible |= gDispCntShadow;
    gDispCntShadow = visible;
}
static inline void BackgroundFlash_HideLayer(void)
{
    u16 *control = &gDispCntShadow;
    u32 mask = ~0x200;
    u32 flags = *control;
    mask &= flags;
    *control = mask;
}
#endif
