#include "background_flash.h"
#include "monster_particle_random.h"
void BackgroundFlash_LoadPalette(void);
void BackgroundFlash_SetState(struct BackgroundFlashController *, u32);
s32 BackgroundFlash_Initialize(struct BackgroundFlashController *controller, void *sourceA,
                               void *sourceB)
{
    u8 *resource = sub_08216E54(0xEA77);
    void *map = Video_GetBackgroundMap(1);
    u32 offset;
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
    {
        const u16 *table = gRandomTable;
        controller->retargetTimer = (ParticleRandom_ReadDirectSample(table) & 127) + 128;
        controller->speed = (ParticleRandom_ReadDirectSample(table) & 7) - 4;
        controller->targetSpeed = (ParticleRandom_ReadDirectSample(table) & 7) - 4;
    }
    BackgroundFlash_HideLayer();
    BackgroundFlash_SetState(controller, 0);
    return 0;
}
