#include "global.h"
#include "ram.h"
#include "area_effect_link.h"
#define SpriteGroup_Initialize PanelSequence_InitializeSprites
struct PanelSequence {
    u8 pad0[0x1C];
    void *palette;
    u8 pad20[0x304];
    u8 indicators[0x10C4];
    void (*update)(struct PanelSequence *);
    u32 source, context, names;
    u8 pad13F8[4];
    u8 transition;
    u8 count;
    u8 pad13FE[0xE];
    u8 entering;
    u8 pad140D[5];
    u16 option;
};
s32 Script_SeekToKeyword(u32);
u32 Script_GetTextSourcePc(void);
s32 Script_GetPc(void);
s32 Script_GetKeywordValue(u32, s32);
void Bg_ClearTilemap(s32);
void Registry30AC_InitBackgroundResources(void *);
void SlotMenu_InitializeAlternateEntryValues(void *);
void PanelSequence_InitializeSprites(void *);
void sub_0810CFF0(void *);
void sub_0810CF90(void *);
void ModifierMenu_InitSelectionSnapshot(void *, void *);
void LinkPanel_CreateTextWindows(void *);
void LinkPanel_StartDescriptions(void *);
void PanelSequence_UpdateDelay(struct PanelSequence *);
void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);
static inline u8 *GetSelectionCount(void)
{
    struct AreaEffectLinkState *state = gLinkSession;
    u32 result;
    if (!state)
        return 0;
    result = state->unavailable;
    if (result != 1)
        return 0;
    result = (u32)state->outgoingChoices;
    return (u8 *)result;
}

static inline void PanelSequence_SetUpdate(struct PanelSequence *s,
                                           void (*update)(struct PanelSequence *))
{
    s->update = update;
}
static inline u8 PanelSequence_ClampSelectionCount(u8 value, s32 min, s32 max)
{
    if (value > max)
        value = max;
    else if (value < min)
        value = min;
    return value;
}

s32 PanelSequence_InitializeFromScript(struct PanelSequence *s)
{
    u8 *count;
    s32 selection;

    if (Script_SeekToKeyword('s')) {
        s->source = Script_GetTextSourcePc();
        if (!s->source)
            return -1;
    }
    if (Script_SeekToKeyword('c')) {
        s->context = Script_GetTextSourcePc();
        if (!s->context)
            return -1;
    }
    if (Script_SeekToKeyword('n')) {
        s->names = Script_GetPc();
        if (!s->names)
            return -1;
    }
    s->option = Script_GetKeywordValue('e', 0);
    count = GetSelectionCount();
    if (!count)
        return -1;
    s->count = *count;
    selection = PanelSequence_ClampSelectionCount(s->count, 0, 10);
    {
        s->count = selection;
    }
    Bg_ClearTilemap(3);
    Bg_ClearTilemap(2);
    Bg_ClearTilemap(1);
    Bg_ClearTilemap(0);
    Registry30AC_InitBackgroundResources(s);
    SlotMenu_InitializeAlternateEntryValues(s);
    PanelSequence_InitializeSprites(s);
    sub_0810CFF0(s);
    sub_0810CF90(s);
    ModifierMenu_InitSelectionSnapshot(s->indicators, s->palette);
    LinkPanel_CreateTextWindows(s);
    LinkPanel_StartDescriptions(s);
    PanelSequence_SetUpdate(s, PanelSequence_UpdateDelay);
    s->entering = 1;
    s->transition = 0;
    if (Script_GetKeywordValue('f', 0))
        ScreenTransition_SendCommand(2, 5, 4, 4, 4, 0xFFFF, 0);
    gPanelSequence = s;
    return 1;
}
