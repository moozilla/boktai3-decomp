#include "global.h"
#include "ui/pause/inventory_grid.h"
#include "ui/pause/scene_hud_display.h"
#include "engine/video/sprite_drawable_api.h"
#include "bosses/vanargand/vanargand_parts.h"

s32 PauseMenu_TickPaletteBlend(void *);

void SceneHud_BeginRandomActionReturn(void);

void Hud_SetInteractionCallback(void *, void (*)(void), int);

void InventoryGrid_UpdateNumberPhase(struct SceneHudState *s)
{
    if (s->actionTransferDelay <= 31) {
        if (++s->actionTransferDelay == 32) {
            s32 i;
            for (i = 0; i < 4; i++)
                s->numberDigits[i].flags |= 1;
        } else if (s->actionTransferDelay <= 8) {
            s32 i;
            for (i = 0; i < 4; i++)
                s->numberDigits[i].position.y--;
        }
    } else if (PauseMenu_TickPaletteBlend(s)) {
        Hud_SetInteractionCallback(s, SceneHud_BeginRandomActionReturn, 1);
    }
}

u32 PauseMenu_UpdateSpriteMotion(void *, u32);

u32 PauseMenu_GetPlayerStatusValue(void *);

void PauseMenu_StartPaletteTransition(void *, u32, u32, s32);

void PauseMenu_ApplyItemSlot(void *, u32);

void Sound_PlaySong(u32);

void InventoryGrid_BeginNumberPhase(struct SceneHudState *s)
{
    struct ContactPosition vec;
    if (PauseMenu_UpdateSpriteMotion(s, 0)) {
        if (++s->actionTransferDelay > 7) {
            s->motionWidgets[0].flags |= 1;
            PauseMenu_ApplyItemSlot(s, s->actionSelection);
            PauseMenu_StartPaletteTransition(s, 0x16E, PauseMenu_GetPlayerStatusValue(s), 16);
            Vanargand_SetVector(&vec, 32, 48, 0);
            InventoryGrid_ShowNumber(s, s->appliedStatB, &vec);
            Sound_PlaySong(0x10D);
            Hud_SetInteractionCallback(s, InventoryGrid_UpdateNumberPhase, 1);
        }
    }
}

void SceneHud_GetActionIconPosition(void *, int);

s32 Inventory_GetQuantityByStorage(u32, u32);

u32 Selection_GetDisplayFrame(s32);

void PauseMenu_BeginSpriteMotion(void *, u32, s32, u32, void *, void *, u32, u32);

void InventoryGrid_BeginIconTransfer(struct SceneHudState *s)
{
    struct ContactPosition from, to;
    u8 *kind;
    u32 index;
    u32 later;
    u32 *flags;
    s32 result;
    kind = &s->actionSelection;
    index = *kind + 10;
    {
        u32 off = index * sizeof(struct AnimSprite);
        u8 *base = (u8 *)&s->categoryWidgets[0].flags;
        flags = (u32 *)(base + off);
    }
    *flags |= 1;
    SceneHud_GetActionIconPosition(&from, s->actionCursor);
    to.x = 16;
    to.y = 48;
    to.z = 0;
    result = Selection_GetDisplayFrame(
        Inventory_GetQuantityByStorage(s->actionCategory, *kind));
    later = *kind + 10;
    PauseMenu_BeginSpriteMotion(s, 0, result, s->categoryWidgets[later].paletteBank, &from, &to,
                             24, 16);
    Sound_PlaySong(0x38A);
    Hud_SetInteractionCallback(s, InventoryGrid_BeginNumberPhaseAlternate, 1);
}

s32 Div(s32, s32);

void InventoryGrid_ShowNumber(struct SceneHudState *s, s32 value,
                              const struct ContactPosition *origin)
{
    s32 digits[4];
    s32 first, i, remainder;
    struct ContactPosition position;
    struct SpriteDrawable *digit;
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
        digit = s->numberDigits;
        do {
            if (i < first)
                digit->flags |= 1;
            else {
                digit->flags &= ~1;
                SpriteDrawable_SelectTileFrame(&s->numberDigits[i], s->numberDigitResource,
                                               *number + 42);
                digit->position = position;
                position.x += 5;
            }
            number++;
            digit++;
        } while (++i <= 3);
    }
}

void InventoryGrid_BeginNumberPhaseAlternate(struct SceneHudState *s)
{
    struct ContactPosition vec;
    if (PauseMenu_UpdateSpriteMotion(s, 0)) {
        if (++s->actionTransferDelay > 7) {
            s->motionWidgets[0].flags |= 1;
            PauseMenu_ApplyItemSlot(s, s->actionSelection);
            PauseMenu_StartPaletteTransition(s, 0x16E, PauseMenu_GetPlayerStatusValue(s), 16);
            Vanargand_SetVector(&vec, 32, 48, 0);
            InventoryGrid_ShowNumber(s, s->appliedStatB, &vec);
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

void InventoryGrid_FinishNumberPhase(struct SceneHudState *s)
{
    if (s->actionTransferDelay <= 31) {
        if (++s->actionTransferDelay == 32) {
            s32 i;
            for (i = 0; i < 4; i++)
                s->numberDigits[i].flags |= 1;
        } else if (s->actionTransferDelay <= 8) {
            s32 i;
            for (i = 0; i < 4; i++)
                s->numberDigits[i].position.y--;
        }
    } else if (PauseMenu_TickPaletteBlend(s)) {
        void *selection;
        SceneHud_RefreshActionIcons(s);
        Hud_SetInteractionCallback(s, SceneHud_UpdateActionSelection, 1);
        selection = s->actionInput;
        Inventory_InitializeSelectionGrid(selection, 1, 0);
        SceneHud_GetActionIconPosition(&s->categoryWidgets[5].position, s->actionCursor);
        SceneHud_ShowActionPreview(s);
        Hud_SaveGridSelection(selection);
        InventoryPanel_ShowStatusIcons(s);
        MenuDisplay_UpdateActionPrompt(s, 4);
        InventoryPanel_DrawStatComparison(s, 0, 0, 0, 1, 0);
        MenuDisplay_SetPaletteOverrides(s, 1, 0, 0, 0);
    }
}
