#include "global.h"
#include "ui/pause/scene_hud_display.h"
#include "player/player.h"
#include "engine/video/render.h"
#include "engine/word_flags.h"
#include "link/session/link_session_state.h"
u16 *Hud_GetTilemapCell(s32, u32, u32);
static inline u8 slotCount(s32 slot)
{
    if (!gLinkSession || slot < 0)
        return 0;
    return gLinkSession->playerValues[slot];
}
void SceneHud_DrawSlotMeter(struct SceneHudState *hud, s32 mode, u32 x, u32 y)
{
    u16 *tiles = Hud_GetTilemapCell(0, x, y);
    u32 pulse;
    s32 count, i;
    if (mode == 1)
        pulse = 0x29F;
    else if (WordFlags_Test(&hud->actionOwner->statusFlags, 0x10)) {
        u32 frame = hud->meterPulseFrame;
        if (frame <= 0x24)
            pulse = 0x29F;
        else if (frame <= 0x2C)
            pulse = 0x1FB;
        else if (frame <= 0x34)
            pulse = 0x177;
        else
            pulse = 0x1FB;
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
