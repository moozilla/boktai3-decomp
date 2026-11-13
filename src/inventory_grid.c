#include "global.h"
#include "inventory_grid.h"

s32 SceneHud_TickPaletteBlend(void *);

void SceneHud_BeginRandomActionReturn(void);

void Hud_SetInteractionCallback(void *, void (*)(void), int);

void InventoryGrid_UpdateNumberPhase(struct InventoryGrid *s)
{
    if (s->view.state.phaseCounter <= 31) {
        if (++s->view.state.phaseCounter == 32) {
            u32 mask = 1;
            struct InventoryNumberDigit *p = s->view.state.digits;
            s32 i = 3;
            do {
                p->flags |= mask;
                p++;
            } while (--i >= 0);
        } else if (s->view.state.phaseCounter <= 8) {
            u16 *p = &s->view.state.digits[0].position.y;
            s32 i = 3;
            do {
                (*p)--;
                p += 0x14;
            } while (--i >= 0);
        }
    } else if (SceneHud_TickPaletteBlend(s)) {
        Hud_SetInteractionCallback(s, SceneHud_BeginRandomActionReturn, 1);
    }
}

u32 PauseMenu_UpdateSpriteMotion(void *, u32);

u32 PlayerPanel_GetStatusValue(void *);

void PauseMenu_StartPaletteTransition(void *, u32, u32, s32);

void PauseMenu_ApplyItemSlot(void *, u32);

void InventoryGrid_ShowNumber(struct InventoryGrid *, s32, struct InventoryGridVector *);

void InventoryGrid_UpdateNumberPhase(struct InventoryGrid *);

void Sound_PlaySong(u32);

static inline void SetVector(struct InventoryGridVector *v, s32 x, s32 y, s32 z)
{
    v->x = x;
    v->y = y;
    v->z = z;
}

void InventoryGrid_BeginNumberPhase(struct InventoryGrid *s)
{
    struct InventoryGridVector vec;
    if (PauseMenu_UpdateSpriteMotion(s, 0)) {
        if (++s->view.state.phaseCounter > 7) {
            s->view.state.motionFlags |= 1;
            PauseMenu_ApplyItemSlot(s, s->view.state.selectedItemSlot);
            PauseMenu_StartPaletteTransition(s, 0x16E, PlayerPanel_GetStatusValue(s), 16);
            SetVector(&vec, 32, 48, 0);
            InventoryGrid_ShowNumber(s, s->view.state.appliedValue, &vec);
            Sound_PlaySong(0x10D);
            Hud_SetInteractionCallback(s, InventoryGrid_UpdateNumberPhase, 1);
        }
    }
}

void SceneHud_GetActionIconPosition(void *, int);

s32 Inventory_GetQuantityByStorage(u32, u32);

u32 Selection_GetDisplayFrame(s32);

void SceneHud_BeginIconMotion(void *, u32, s32, u32, void *, void *, u32, u32);

void InventoryGrid_BeginNumberPhaseAlternate(struct InventoryGrid *);

void InventoryGrid_BeginIconTransfer(struct InventoryGrid *s)
{
    struct InventoryGridVector from, to;
    u8 *kind;
    u32 index;
    u32 later;
    u32 *flags;
    s32 result;
    kind = &s->view.state.selectedItemSlot;
    index = *kind + 10;
    {
        u32 off = index * sizeof(struct InventoryGridElement);
        u8 *base = (u8 *)s->view.elementView.elements;
        flags = (u32 *)(base + off);
    }
    *flags |= 1;
    SceneHud_GetActionIconPosition(&from, s->view.state.selectedSlot);
    to.x = 16;
    to.y = 48;
    to.z = 0;
    result = Selection_GetDisplayFrame(
        Inventory_GetQuantityByStorage(s->view.state.itemStorage, *kind));
    later = *kind + 10;
    SceneHud_BeginIconMotion(s, 0, result, s->view.elementView.elements[later].value, &from, &to,
                             24, 16);
    Sound_PlaySong(0x38A);
    Hud_SetInteractionCallback(s, InventoryGrid_BeginNumberPhaseAlternate, 1);
}

s32 Div(s32, s32);

void SpriteDrawable_SelectTileFrame(void *, void *, u32);

void InventoryGrid_ShowNumber(struct InventoryGrid *s, s32 value,
                              struct InventoryGridVector *origin)
{
    s32 digits[4];
    s32 first, i, remainder;
    struct InventoryGridVector position;
    struct InventoryNumberDigit *digit;
    s32 *number;
    if (value > 0) {
        if (value > 9999)
            value = 9999;
        digits[0] = Div(value, 1000);
        remainder = value - digits[0] * 1000;
        digits[1] = Div(remainder, 100);
        remainder -= digits[1] * 100;
        digits[2] = Div(remainder, 10);
        remainder -= digits[2] * 10;
        digits[3] = remainder;
        if (digits[0] > 0)
            first = 0;
        else if (digits[1] > 0)
            first = 1;
        else if (digits[2] > 0)
            first = 2;
        else
            first = 3;
        position = *origin;
        position.x -= (4 - first) * 4;
        position.y -= 4;
        i = 0;
        number = digits;
        digit = s->view.state.digits;
        do {
            if (i < first)
                digit->flags |= 1;
            else {
                digit->flags &= ~1;
                SpriteDrawable_SelectTileFrame(&s->view.state.digits[i], s->view.state.digitResource, *number + 42);
                digit->position = position;
                position.x += 5;
            }
            number++;
            digit++;
        } while (++i <= 3);
    }
}

void InventoryGrid_FinishNumberPhase(struct InventoryGrid *);

void InventoryGrid_BeginNumberPhaseAlternate(struct InventoryGrid *s)
{
    struct InventoryGridVector vec;
    if (PauseMenu_UpdateSpriteMotion(s, 0)) {
        if (++s->view.state.phaseCounter > 7) {
            s->view.state.motionFlags |= 1;
            PauseMenu_ApplyItemSlot(s, s->view.state.selectedItemSlot);
            PauseMenu_StartPaletteTransition(s, 0x16E, PlayerPanel_GetStatusValue(s), 16);
            SetVector(&vec, 32, 48, 0);
            InventoryGrid_ShowNumber(s, s->view.state.appliedValue, &vec);
            Sound_PlaySong(0x10D);
            Hud_SetInteractionCallback(s, InventoryGrid_FinishNumberPhase, 1);
        }
    }
}

void SceneHud_UpdateActionSelection(void);

void MenuDisplay_SetPaletteOverrides(void *, int, int, int, int);

void MenuDisplay_UpdateActionPrompt(void *, int);

void InventoryPanel_DrawStatComparison(void *, int, int, int, int, int);

void InventoryPanel_ShowStatusIcons(void *);

void Hud_SaveGridSelection(void *);

void Inventory_InitializeSelectionGrid(void *, int, int);

void SceneHud_ShowActionPreview(void *);

void SceneHud_RefreshActionIcons(void *);

void InventoryGrid_FinishNumberPhase(struct InventoryGrid *s)
{
    if (s->view.state.phaseCounter <= 31) {
        if (++s->view.state.phaseCounter == 32) {
            u32 mask = 1;
            struct InventoryNumberDigit *p = s->view.state.digits;
            s32 i = 3;
            do {
                p->flags |= mask;
                p++;
            } while (--i >= 0);
        } else if (s->view.state.phaseCounter <= 8) {
            u16 *p = &s->view.state.digits[0].position.y;
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
        selection = s->view.state.selectionState;
        Inventory_InitializeSelectionGrid(selection, 1, 0);
        SceneHud_GetActionIconPosition(s->view.state.slotPosition, s->view.state.selectedSlot);
        SceneHud_ShowActionPreview(s);
        Hud_SaveGridSelection(selection);
        InventoryPanel_ShowStatusIcons(s);
        MenuDisplay_UpdateActionPrompt(s, 4);
        InventoryPanel_DrawStatComparison(s, 0, 0, 0, 1, 0);
        MenuDisplay_SetPaletteOverrides(s, 1, 0, 0, 0);
    }
}
