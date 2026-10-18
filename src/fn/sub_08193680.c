#include "global.h"
#include "engine.h"

void Bg_SetupLayers(int, int, void *, int, int, int, void *);
void SceneAnimation_SetSlotState(void *, u32, u32);

void Nidhoggr_SetBackgroundMode(u8 *t, u32 mode)
{
    u8 *cur = t + 0x724b;
    if (*cur != mode) {
        u32 index = mode;
        u32 zero = 0;
        *cur = mode;
        Bg_SetupLayers(2, 0, *(void **)(t + 0x50), zero, 0, 1, &index);
        BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
        if (*cur) {
            SceneAnimation_SetSlotState(t, 0x15, 0x33);
            SceneAnimation_SetSlotState(t, 0x14, 0x34);
        } else {
            SceneAnimation_SetSlotState(t, 0x15, 0x10);
            SceneAnimation_SetSlotState(t, 0x14, 0x11);
        }
    }
}
