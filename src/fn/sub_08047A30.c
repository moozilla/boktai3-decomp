// MATCHING_CONCESSION: register-pinning
#include "background_flash.h"
void BackgroundFlash_LoadPalette(void);
void BackgroundFlash_SetState(struct BackgroundFlashController *, u32);
s32 BackgroundFlash_Initialize(struct BackgroundFlashController *controller, void *sourceA,
                               void *sourceB)
{
    u8 *resource = sub_08216E54(0xEA77);
    void *map = Video_GetBackgroundMap(1);
    u32 offset;
    const u16 *table;
    register u32 index asm("r1");
    u32 mask;
    u32 sample;
    register u32 sampleMask asm("r2");
    const u16 *lastSample;
    controller->effectSourceA = sourceA;
    controller->effectSourceB = sourceB;
    controller->xOffsets[0] = 0;
    controller->yOffsets[0] = 0;
    controller->xOffsets[1] = 0;
    controller->yOffsets[1] = 0;
    controller->page = 0;
    if (resource[0] == 'M' && resource[1] == 'P') {
        offset = 0;
        Bg_SetPriority(1, 1);
        Bg_SetupLayers(3, 1, resource, 0, 0, 1, (s32 *)&offset);
    } else {
        Bg_SetupLayer(1, 1, resource, 0, 0, 0, 1, map);
    }
    BackgroundFlash_LoadPalette();
    table = (const u16 *)RANDOM_TABLE_ADDR;
    index = gRandomIndex + 1;
    mask = 1023;
    index &= mask;
    sample = table[index];
    sampleMask = 127;
    sample &= sampleMask;
    sample += 128;
    controller->retargetTimer = sample;
    index = (index + 1) & mask;
    sample = table[index];
    sampleMask = 7;
    sample &= sampleMask;
    sample -= 4;
    controller->speed = sample;
    index = (index + 1) & mask;
    gRandomIndex = index;
    lastSample = (const u16 *)((index << 1) + (u32)table);
    sample = *lastSample;
    sample &= sampleMask;
    sample -= 4;
    controller->targetSpeed = sample;
    BackgroundFlash_HideLayer();
    BackgroundFlash_SetState(controller, 0);
    return 0;
}
