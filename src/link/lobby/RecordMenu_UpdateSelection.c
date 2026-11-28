#include "global.h"
#include "engine/engine.h"
#include "engine/input.h"
#include "gba/io_reg.h"
#include "engine/video/palette_pulse.h"
#include "link/lobby/record_menu.h"

void Bg_SetupLayers(int, int, void *, int, int, int, void *);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void LinkLobbyPanel_ClearTextWindows(struct RecordMenu *);
void LinkLobbyPanel_HideSprites(struct RecordMenu *);
void TextWindow_StartPlayback(s32);
void LinkSession_SetSelectionResponse(s32);
void Sound_PlaySong(u32);
void SceneTask_UpdateTextDelay(struct RecordMenu *);

/* The yes/no prompt: left and right move the cursor, A answers, B answers no. */
void RecordMenu_UpdateSelection(struct RecordMenu *menu)
{
    u32 layer;

    if (menu->redraw) {
        LinkLobbyPanel_ClearTextWindows(menu);
        LinkLobbyPanel_HideSprites(menu);
        Bg_ClearTilemap(0);
        Bg_ClearTilemap(1);
        Bg_ClearTilemap(2);
        Bg_ClearTilemap(3);
        layer = 3;
        Bg_SetupLayers(2, 0, menu->otherMap, 0, 0, 1, &layer);
        BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
        TextWindow_SetBounds(menu->nameWindows[0], 1, 6, 28, 6);
        TextWindow_SetScript(menu->nameWindows[0], menu->titleScript);
        TextWindow_Refresh(menu->nameWindows[0], 7);
        TextWindow_StartPlayback(menu->nameWindows[0]);
        menu->sprites[8].displayFlags &= ~1;
        menu->choice = 1;
        menu->inputDelay = 0;
        menu->redraw = 0;
    }

    Palette_UpdateHighlightPulse(&menu->palette[12], &menu->pulsePhase);
    if (gPadState[1] & DPAD_LEFT) {
        if (!menu->choice) {
            menu->spriteIds[8] = 0x4B;
            menu->choice = 1;
            Sound_PlaySong(0xDC);
            AnimSprite_SelectGlyph(&menu->sprites[8], menu->resource, menu->spriteIds[8]);
        }
    } else if (gPadState[1] & DPAD_RIGHT) {
        if (menu->choice) {
            menu->spriteIds[8] = 0x4C;
            menu->choice = 0;
            Sound_PlaySong(0xDC);
            AnimSprite_SelectGlyph(&menu->sprites[8], menu->resource, menu->spriteIds[8]);
        }
    } else if (gPadState[1] & A_BUTTON) {
        menu->sprites[8].displayFlags |= 1;
        LinkSession_SetSelectionResponse(menu->choice);
        RecordMenu_Begin(menu, SceneTask_UpdateTextDelay, 2);
        if (menu->choice)
            Sound_PlaySong(0xDD);
        else
            Sound_PlaySong(0xDE);
    } else if (gPadState[1] & B_BUTTON) {
        menu->sprites[8].displayFlags |= 1;
        LinkSession_SetSelectionResponse(0);
        RecordMenu_Begin(menu, SceneTask_UpdateTextDelay, 2);
        Sound_PlaySong(0xDE);
    }
}
