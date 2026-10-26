#include "global.h"
#include "record_menu.h"
#include "area_effect_link.h"
#include "ram.h"
// MATCHING_CONCESSION: register-pinning
s32 Script_SeekToKeyword(u32);
u32 Script_GetTextSourcePc(void);
u32 Script_GetPc(void);
s32 Script_GetKeywordValue(u32, s32);
void Bg_ClearTilemap(s32);
void Registry33FF_LoadBackground(struct RecordMenu *);
void LinkLobbyPanel_InitGlyphFrames(struct RecordMenu *);
void RecordMenu_InitializeSprites(struct RecordMenu *);
void RecordMenu_LoadPlayerSprites(struct RecordMenu *);
void RecordMenu_PositionSprites(struct RecordMenu *);
void ModifierMenu_InitSelectionSnapshot(void *, void *);
void Registry33FF_CreateTextWindows(struct RecordMenu *);
void LinkLobbyPanel_RefreshPlayerNames(struct RecordMenu *);
void RecordMenu_UpdateRecords(struct RecordMenu *);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
static inline u8 *GetChoices(void)
{
    struct AreaEffectLinkState *state = gUnk_020004B4;
    u32 result;
    if (!state)
        return 0;
    result = state->unavailable;
    if (result != 1)
        return 0;
    result = (u32)state->outgoingChoices;
    return (u8 *)result;
}
static inline s32 SetSelection(struct RecordMenu *menu, s32 value)
{
    u8 *field = &menu->selection;
    s32 zero = 0;
    /* Keep the cleared flag available throughout initialization. */
    asm("" : "+r"(zero));
    *field = value;
    return zero;
}
static inline s32 LimitSelection(s32 value)
{
    if (value > 5)
        value = 5;
    return value;
}
s32 RecordMenu_Initialize(struct RecordMenu *menu)
{
    u8 *choices;
    s32 selection;
    s32 zero;
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
    {
        s32 choice = choices[0];
        u8 *field = &menu->selection;
        *field = choice - 5;
    }
    selection = LimitSelection(menu->selection);
    {
        zero = SetSelection(menu, selection);
        Bg_ClearTilemap(3);
        Bg_ClearTilemap(2);
        Bg_ClearTilemap(1);
        Bg_ClearTilemap(0);
        Registry33FF_LoadBackground(menu);
        LinkLobbyPanel_InitGlyphFrames(menu);
        RecordMenu_InitializeSprites(menu);
        RecordMenu_LoadPlayerSprites(menu);
        RecordMenu_PositionSprites(menu);
        ModifierMenu_InitSelectionSnapshot(menu->textPanel, menu->paletteResource);
        Registry33FF_CreateTextWindows(menu);
        LinkLobbyPanel_RefreshPlayerNames(menu);
        {
            void (*next)(struct RecordMenu *) = RecordMenu_UpdateRecords;
            menu->update = next;
        }
        menu->redraw = 1;
        {
            menu->leaving = zero;
            if (Script_GetKeywordValue('f', 0))
                ScreenTransition_SendCommand(2, 5, 4, 4, 4, 65535, zero);
        }
    }
    gActiveRecordMenu = menu;
    return 1;
}
