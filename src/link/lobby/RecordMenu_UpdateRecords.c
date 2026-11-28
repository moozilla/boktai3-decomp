#include "global.h"
#include "engine/engine.h"
#include "engine/input.h"
#include "gba/io_reg.h"
#include "link/lobby/record_menu.h"

void Bg_SetLayerMap(u32, void *, u16);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void Sound_PlaySong(u32);
void RecordMenu_DrawRecords(struct RecordMenu *);
void LinkLobbyPanel_DrawActorValues(struct RecordMenu *);
void CharacterModel_UpdatePairedAnimations(void *, s32);
void PaletteFade_CycleMenuColors(void *);
void RecordMenu_UpdateSelection(struct RecordMenu *);

void RecordMenu_UpdateRecords(struct RecordMenu *menu)
{
    s32 i;

    if (menu->redraw) {
        Bg_SetLayerMap(3, menu->backgroundMap, 2);
        BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
        RecordMenu_DrawRecords(menu);
        LinkLobbyPanel_DrawActorValues(menu);
        menu->inputDelay = 0;
        menu->redraw = 0;
    }
    for (i = 0; i < 2; i++)
        CharacterModel_UpdatePairedAnimations(&menu->previews[i].model, i);
    PaletteFade_CycleMenuColors(&menu->paletteCycle);
    if (menu->inputDelay <= 59) {
        menu->inputDelay++;
    } else if (gPadState[1] & (A_BUTTON | B_BUTTON | START_BUTTON)) {
        Sound_PlaySong(0xDD);
        RecordMenu_Begin(menu, RecordMenu_UpdateSelection, 1);
    }
}
