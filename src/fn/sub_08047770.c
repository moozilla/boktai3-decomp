#include "render.h"
// MATCHING_CONCESSION: register-pinning
#include "background_flash.h"
void BackgroundFlash_UpdateScrolling(struct BackgroundFlashController *controller)
{
    s32 cameraX = gUnk_03005418[0] + gCamera.offsetX;
    s32 cameraY = gUnk_03005418[1] + gCamera.offsetY;
    register u32 frame asm("r1");
    s32 x, y, tileX, tileY;
    if (!controller->retargetTimer) {
        const u16 *table = (const u16 *)RANDOM_TABLE_ADDR;
        register u32 index asm("r0");
        u32 mask;
        u32 sample;
        index = gRandomIndex + 1;
        mask = 1023;
        index &= mask;
        sample = table[index];
        sample &= 127;
        sample += 128;
        controller->retargetTimer = sample;
        index = (index + 1) & mask;
        gRandomIndex = index;
        index = table[index];
        index &= 7;
        index -= 1;
        controller->targetSpeed = index;
    } else {
        --controller->retargetTimer;
    }
    frame = controller->frame;
    if (!(frame & 15)) {
        if (controller->speed < controller->targetSpeed)
            ++controller->speed;
        else if (controller->speed > controller->targetSpeed)
            --controller->speed;
    }
    {
        u32 odd = 1;
        odd &= frame;
        if (odd) {
            controller->xOffsets[0] += 3 + controller->speed;
            controller->yOffsets[0] -= 8 + controller->speed;
        } else {
            controller->xOffsets[1] += 6 + controller->speed;
            controller->yOffsets[1] -= 16 + controller->speed;
        }
    }
    x = (cameraX + controller->xOffsets[controller->page]) & 255;
    y = (cameraY + controller->yOffsets[controller->page]) & 255;
    tileX = Div(cameraX - 128, 8);
    tileY = Div(cameraY - 128, 8) & ~1;
    BgGfx_LoadTilemapSimple(1, tileX, tileY, x, y);
}
