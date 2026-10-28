#include "global.h"
#include "scene_hud_display.h"
#include "player.h"
#include "render.h"
#define SceneHud_GetTilemapCell Hud_GetTilemapCell
struct SceneHudSlotCounts {
    u8 unknown0[0x5C];
    u8 counts[4];
};
extern struct SceneHudSlotCounts *gUnk_020004B4;
u16 *SceneHud_GetTilemapCell(s32, u32, u32);
static inline u32 testFlags(const u32 *flags, u32 mask)
{
    return *flags & mask;
}
static inline u8 slotCount(s32 slot)
{
    if (!gUnk_020004B4 || slot < 0)
        return 0;
    return gUnk_020004B4->counts[slot];
}
void SceneHud_DrawSlotMeter(struct SceneHudState *hud, s32 mode, u32 x, u32 y)
{
    u16 *tiles = SceneHud_GetTilemapCell(0, x, y);
    u32 pulse;
    s32 count, i;
    if (mode == 1)
        pulse = 0x29F;
    else if (testFlags(&hud->actionOwner->statusFlags, 0x10)) {
        u32 frame = hud->meterPulseFrame;
        pulse = 0x29F;
        if (frame > 0x24) {
            pulse = 0x1FB;
            if (frame <= 0x2C)
                goto pulse_ready;
            if (frame <= 0x34)
                pulse = 0x177;
        }
    pulse_ready:
        if (++hud->meterPulseFrame > 0x3B)
            hud->meterPulseFrame = 0;
    } else {
        hud->meterPulseFrame = 0x34;
        pulse = 0x177;
    }
    if (gBgPaletteBank13Color10 != pulse)
        gBgPaletteBank13Color10 = pulse;
    count = slotCount(hud->actionOwner->slot);
    *tiles++ = 0xD45F;
    for (i = 0; i < count; i++)
        *tiles++ = 0xD05D;
    for (; i <= 9; i++)
        *tiles++ = 0xD05E;
    *tiles = 0xD05F;
}
