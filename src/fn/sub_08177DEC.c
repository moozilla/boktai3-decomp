#include "global.h"
#include "ram.h"
struct HudElement { /* 0x60 bytes */
    u8 pad0[8];
    u32 flags;
    u8 padC[0x54];
};
extern u8 *gGameState;
extern u32 gEngineFlags;
void Hud_SetElementGroupFlags(u8 *);
void Hud_HideAuxiliaryElements(u8 *);
void Bg_ClearTilemap(s32);
void Hud_SetupBackgroundLayer(u8 *, void *, s32, s32, s32);
void SceneHud_RefreshWidgetVisibility(u8 *);
void MessageWindow_SetPalette(s32);
void PauseMenu_RefreshStatusPanel(u8 *);
/* The HUD block is large and mostly unmapped; offsets are read raw. */
#define HUD_U8(o)  (*(u8 *)(hud + (o)))
#define HUD_U16(o) (*(u16 *)(hud + (o)))
#define HUD_U32(o) (*(u32 *)(hud + (o)))
#define HUD_PTR(o) (*(void **)(hud + (o)))
/* Records HUD mode (+0xA5E) and applies it unless the HUD is busy
   (+0xA5C). Modes 7, 8 and 3 and up set bit 0 in four element groups'
   flags; 8 also clears it in eight more elements; 9 uses row 2, 0-2 row 0
   and 3 and up row mode - 2 with sound EFDA in Hud_SetupBackgroundLayer (tentative). */
void Hud_SetMode(u32 mode)
{
    u8 *hud = gFieldHud;
    u8 *current;
    u16 busy;
    if (!hud)
        return;
    current = &HUD_U8(0xA5E);
    *current = mode;
    busy = HUD_U16(0xA5C);
    if (busy)
        return;
    if ((u8)mode == 7) {
        Hud_SetElementGroupFlags(hud);
        ((struct HudElement *)(hud + 0xA9C))->flags |= 1;
        ((struct HudElement *)(hud + 0xB08))->flags |= 1;
        ((struct HudElement *)(hud + 0xB68))->flags |= 1;
        ((struct HudElement *)(hud + 0xBC8))->flags |= 1;
        HUD_U8(0xA9B) = 0;
        Hud_HideAuxiliaryElements(hud);
        Bg_ClearTilemap(0);
    } else if ((u8)mode == 8) {
        struct HudElement *e;
        s32 i;
        Hud_SetElementGroupFlags(hud);
        HUD_U32(0xAA4) |= 1;
        HUD_U32(0xB10) |= 1;
        HUD_U32(0xB70) |= 1;
        HUD_U32(0xBD0) |= 1;
        HUD_U8(0xA9B) = 0;
        {
            s32 mask;
            e = (struct HudElement *)(hud + 0x4988);
            mask = ~1;
            for (i = 1; i >= 0; e++, i--)
                e->flags &= mask;
            e = (struct HudElement *)(hud + 0x4A70);
            mask = ~1;
            for (i = 5; i >= 0; e++, i--)
                e->flags &= mask;
        }
        Bg_ClearTilemap(0);
        Hud_SetupBackgroundLayer(hud, HUD_PTR(0x28), 0, 0, HUD_U32(0x38));
    } else if ((u8)mode == 9) {
        Hud_SetupBackgroundLayer(hud, HUD_PTR(0x18), 0, 2, HUD_U32(0x2C));
    } else {
        s32 shown = 0;
        s32 row;
        if ((u8)mode <= 2) {
            if (!(gEngineFlags & 0x1000))
                SceneHud_RefreshWidgetVisibility(hud);
            if (*current == 1) {
                u32 value = *(u32 *)(gGameState + 0x618);
                HUD_U16(0xA88) = value;
            }
            row = 0;
        } else {
            Hud_SetElementGroupFlags(hud);
            HUD_U32(0xAA4) |= 1;
            HUD_U32(0xB10) |= 1;
            HUD_U32(0xB70) |= 1;
            HUD_U32(0xBD0) |= 1;
            row = *current - 2;
            shown = 1;
            HUD_U8(0xA9B) = 0;
            Hud_HideAuxiliaryElements(hud);
        }
        Hud_SetupBackgroundLayer(hud, HUD_PTR(0x18), 0, row, HUD_U32(0x2C));
        if (shown)
            MessageWindow_SetPalette(0xEFDA);
        if (gEngineFlags & 0x1000)
            PauseMenu_RefreshStatusPanel(hud);
    }
    HUD_U8(0xA8E) = 0;
}
