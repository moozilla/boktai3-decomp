#define InventoryGrid_SetPhaseCallback Hud_SetInteractionCallback
#include "global.h"

struct InventoryNumberDigit {
    u32 flags;
    u8 unknown04[0x14];
    struct InventoryNumberPosition {
        s16 x, y, z, pad;
    } position;
    u8 unknown20[8];
};
struct InventoryGrid {
    u8 unknown0000[0xA4E];
    u16 phaseCounter;
    u8 unknown0A50[0x1620 - 0xA50];
    s16 slotPosition[4];
    u8 unknown1628[0x4384 - 0x1628];
    u8 selectionState[3];
    u8 selectedSlot;
    u8 unknown4388[0x4CE0 - 0x4388];
    struct InventoryNumberDigit attachments[4];
};
s32 SceneHud_TickPaletteBlend(void *);
void SceneHud_UpdateActionSelection(void);
void MenuDisplay_SetPaletteOverrides(void *, int, int, int, int);
void MenuDisplay_UpdateActionPrompt(void *, int);
void InventoryPanel_DrawStatComparison(void *, int, int, int, int, int);
void InventoryPanel_ShowStatusIcons(void *);
void Hud_SaveGridSelection(void *);
void sub_0816CDB8(void *, int, int);
void SceneHud_ShowActionPreview(void *);
void SceneHud_GetActionIconPosition(void *, int);
void SceneHud_RefreshActionIcons(void *);
void Hud_SetInteractionCallback(void *, void (*)(void), int);
void InventoryGrid_FinishNumberPhase(struct InventoryGrid *s)
{
    if (s->phaseCounter <= 31) {
        if (++s->phaseCounter == 32) {
            u32 mask = 1;
            struct InventoryNumberDigit *p = s->attachments;
            s32 i = 3;
            do {
                p->flags |= mask;
                p++;
            } while (--i >= 0);
        } else if (s->phaseCounter <= 8) {
            u16 *p = &s->attachments[0].position.y;
            s32 i = 3;
            do {
                (*p)--;
                p += 0x14;
            } while (--i >= 0);
        }
    } else if (SceneHud_TickPaletteBlend(s)) {
        void *selection;
        SceneHud_RefreshActionIcons(s);
        Hud_SetInteractionCallback(s, SceneHud_UpdateActionSelection, 1);
        selection = s->selectionState;
        sub_0816CDB8(selection, 1, 0);
        SceneHud_GetActionIconPosition(s->slotPosition, s->selectedSlot);
        SceneHud_ShowActionPreview(s);
        Hud_SaveGridSelection(selection);
        InventoryPanel_ShowStatusIcons(s);
        MenuDisplay_UpdateActionPrompt(s, 4);
        InventoryPanel_DrawStatComparison(s, 0, 0, 0, 1, 0);
        MenuDisplay_SetPaletteOverrides(s, 1, 0, 0, 0);
    }
}
