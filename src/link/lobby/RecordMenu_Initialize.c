#include "global.h"
#include "link/lobby/record_menu.h"
#include "scene/areas/area_effect_link.h"
#include "engine/ram.h"
const u8 *Script_GetTextSourcePc(void);
void Bg_ClearTilemap(s32);
void LinkLobbyPanel_LoadBackground(struct RecordMenu *);
void LinkLobbyPanel_InitGlyphFrames(struct RecordMenu *);
void RecordMenu_InitializeSprites(struct RecordMenu *);
void RecordMenu_LoadPlayerSprites(struct RecordMenu *);
void RecordMenu_PositionSprites(struct RecordMenu *);
void ModifierMenu_InitSelectionSnapshot(void *, void *);
void LinkLobbyPanel_CreateTextWindows(struct RecordMenu *);
void LinkLobbyPanel_RefreshPlayerNames(struct RecordMenu *);
void RecordMenu_UpdateRecords(struct RecordMenu *);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
static inline u8 *GetChoices(void)
{
    if (!gLinkSession || gLinkSession->unavailable != 1)
        return NULL;
    return gLinkSession->outgoingChoices;
}

static inline u8 LimitSelection(u8 value, s32 min, s32 max)
{
    if (value > max)
        value = max;
    else if (value < min)
        value = min;
    return value;
}

s32 RecordMenu_Initialize(struct RecordMenu *menu)
{
    u8 *choices;

    if (Script_SeekToKeyword('s')) {
        menu->titleScript = Script_GetTextSourcePc();
        if (!menu->titleScript)
            return -1;
    }
    if (Script_SeekToKeyword('c')) {
        menu->choiceScript = Script_GetTextSourcePc();
        if (!menu->choiceScript)
            return -1;
    }
    if (Script_SeekToKeyword('n')) {
        menu->recordScript = Script_GetPc();
        if (!menu->recordScript)
            return -1;
    }
    menu->exitScript = Script_GetKeywordValue('e', 0);
    choices = GetChoices();
    if (!choices)
        return -1;
    menu->selection = choices[0];
    menu->selection -= 5;
    menu->selection = LimitSelection(menu->selection, 0, 5);
    Bg_ClearTilemap(3);
    Bg_ClearTilemap(2);
    Bg_ClearTilemap(1);
    Bg_ClearTilemap(0);
    LinkLobbyPanel_LoadBackground(menu);
    LinkLobbyPanel_InitGlyphFrames(menu);
    RecordMenu_InitializeSprites(menu);
    RecordMenu_LoadPlayerSprites(menu);
    RecordMenu_PositionSprites(menu);
    ModifierMenu_InitSelectionSnapshot(&menu->paletteCycle, menu->paletteResource);
    LinkLobbyPanel_CreateTextWindows(menu);
    LinkLobbyPanel_RefreshPlayerNames(menu);
    RecordMenu_Begin(menu, RecordMenu_UpdateRecords, 0);
    if (Script_GetKeywordValue('f', 0))
        ScreenTransition_SendCommand(2, 5, 4, 4, 4, 0xFFFF, 0);
    gActiveRecordMenu = (u32)menu;
    return 1;
}
