#include "selection_panel.h"
#include "area_effect_link.h"
#include "scrolling_dialogue.h"
#include "runtime.h"

void TextWindow_StartPlayback(s32);

s32 TextWindow_Clear(s32);

u8 StateFlags_TestIndexedFlag(s32);

void CharacterModel_InitPalettes(void *, s32);

void ActorSprite_InitPartDrawings(void *, void *, u32);

void CharacterPreview_PositionParts(void *, void *, void *, s32);

void CharacterModel_SetAnimationFlags(void *, s32);

void CharacterModel_SetComponentRenderMode(void *, s32);

void SceneActorStyle_InitializeFromGameState(void *);

void CharacterModel_UpdatePairedAnimations(void *, s32);

void CharacterModel_RemoveAnimations(void *);

void PaletteFade_CycleMenuColors(void *);

void Bg_SetLayerMap(u32, void *, u16);

void GameState_SetResultIndex(s32);

void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);

void Registry1745_SetMenuRequest(u32);

s32 Registry1745_GetSessionState(void);

void Registry3C9F_LoadBackground(void *);

void sub_0810D760(void *);

void RecordPanel_ShowName(void *);

void RecordPanel_RefreshEntryLabels(void *);

void RecordPanel_DrawTime(void *);

void Video_DrawClockAndValueOnBackground(u32);

void ModifierMenu_InitSelectionSnapshot(void *, void *);

extern const u8 gUnk_0860654C[];

static inline u8 TestSel(struct SelectionPanel *panel)
{
    if (StateFlags_TestIndexedFlag(SelectionPanel_GetSelection(panel)))
        return 1;
    return 0;
}

static inline u32 LinkActive(void)
{
    u32 mask = 0x800;
    return (gEngineFlags | gScenePauseFlags) & mask;
}

static inline void Refresh(struct SelectionPanel *panel)
{
    if (panel->entering) {
        Bg_SetLayerMap(3, panel->backgroundResource, 1);
        BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
        RecordPanel_DrawTime(panel);
        Video_DrawClockAndValueOnBackground(3);
        panel->timer = 0;
        panel->pulseFrame = 0;
        panel->entering = 0;
    }
}

void SelectionPanel_ClearTextWindows(struct SelectionPanel *panel)
{
    u32 *q;
    s32 i;
    TextWindow_Clear(panel->windows[0]);
    i = 0;
    q = &panel->windows[1];
    do {
        s32 n = i + 1;
        TextWindow_Clear(*q++);
        i = n;
    } while (i <= 5);
}

void SelectionPanel_InitEntryIds(struct SelectionPanel *panel)
{
    panel->entryIds[0] = 0x1e;
    panel->entryIds[2] = 0x2f;
    panel->entryIds[3] = 0x1d;
    panel->entryIds[4] = 1;
    panel->entryIds[5] = 0x21;
    panel->entryIds[6] = 0x26;
    {
        u32 mask = 0x800;
        if ((gEngineFlags | gScenePauseFlags) & mask)
            panel->entryIds[1] = 0x20;
        else
            panel->entryIds[1] = 0x1f;
    }
    panel->entryIds[7] = 0x41;
    panel->entryIds[8] = 0x42;
    panel->entryIds[9] = 0x7f;
}

void SelectionPanel_InitializeWidgets(struct SelectionPanel *panel)
{
    s32 i;
    void *t;
    t = ScriptProc_Start(0xCB05, 0x1289);
    panel->widgetScript = t;
    ResourceHeader_Relocate(&panel->widgetResource, t);
    for (i = 0; i <= 6; i++) {
        Widget_InitLinked(&panel->widgets[i], &panel->widgetResource, panel->entryIds[i], 0x10, 1,
                          0, 0, 0);
        panel->cachedIds[i] = 0xFFFF;
    }
    panel->widgets[3].paletteBank = 0xA008;
    panel->widgets[3].palette = panel->widgetPalette;
    CpuSet(gPaletteBankData + 0x64E0, panel->widgetPalette, 0x04000008);
    t = ScriptProc_Start(0xCB05, 0x530D);
    panel->auxiliaryScript = t;
    ResourceHeader_Relocate(&panel->auxiliaryResource, t);
    for (i = 7; i <= 9; i++) {
        Widget_InitLinked(&panel->widgets[i], &panel->auxiliaryResource, panel->entryIds[i], 0x10,
                          1, 0, 0, 0);
        panel->cachedIds[i] = 0xFFFF;
        {
            u32 mask = 0x800;
            if ((gEngineFlags | gScenePauseFlags) & mask)
                panel->widgets[i].flags |= 1;
        }
    }
    panel->widgets[6].attribute1A = 0;
    panel->widgets[3].attribute1A = 0;
}

void SelectionPanel_PositionWidgets(struct SelectionPanel *panel)
{
    s32 i;
    for (i = 0; i <= 9; i++) {
        switch (i) {
            case 2:
                panel->widgets[i].position.x = 0;
                panel->widgets[i].position.y = -24;
                break;
            case 7:
                panel->widgets[i].position.x = 0x60;
                panel->widgets[i].position.y = 0x10;
                break;
            case 8:
                panel->widgets[i].position.x = 0x60;
                panel->widgets[i].position.y = 0x10;
                break;
            case 9:
                panel->widgets[i].position.x = 8;
                panel->widgets[i].position.y = 8;
                break;
            default:
                panel->widgets[i].position.x = 0;
                panel->widgets[i].position.y = 0;
                break;
        }
    }
}

void SelectionPanel_InitPreview(struct SelectionPanel *panel)
{
    struct SceneInputPackedPosition pos;
    u8 *base, *sprite;
    u32 frame;
    u32 mask = 0x800;
    u32 flags = (gEngineFlags | gScenePauseFlags) & mask;
    if (flags) {
        struct AreaEffectLinkState *source = gLinkSession;
        u8 *values;
        if (!source || source->unavailable != 1)
            return;
        values = source->outgoingPayload;
        if (!values)
            return;
        base = ((u8 *)&panel->preview);
        panel->preview.kind = values[0];
        panel->preview.form = values[2];
        panel->preview.variant = values[1];
        panel->preview.extra = values[4];
        panel->preview.palette = values[3];
        frame = values[3];
        panel->preview.player = 0;
    } else {
        base = ((u8 *)&panel->preview);
        SceneActorStyle_InitializeFromGameState(&panel->preview.kind);
        frame = panel->preview.palette;
        panel->preview.player = flags;
    }
    pos.x = 0x30;
    pos.y = 0x50;
    pos.z = 0;
    sprite = (u8 *)&((struct CharacterPreviewActor *)base)->model;
    ActorSprite_InitPartDrawings(base, sprite, 0x3d99);
    CharacterPreview_PositionParts(base, sprite, &pos, 0);
    CharacterModel_SetAnimationFlags(sprite, 0);
    CharacterModel_SetComponentRenderMode(sprite, 0);
    CharacterModel_InitPalettes(sprite, frame);
}

void SelectionPanel_HideDisplayObjects(struct SelectionPanel *panel)
{
    u32 m = 1;
    u32 *q = &panel->widgets[0].flags;
    s32 i = 9;
    do {
        *q |= m;
        q += sizeof(panel->widgets[0]) / sizeof(*q);
        i--;
    } while (i >= 0);
    CharacterModel_SetAnimationFlags(&panel->preview.model, 1);
}

void SelectionPanel_RemoveWidgets(struct SelectionPanel *panel)
{
    struct SceneInputWidget *q = panel->widgets;
    s32 i;

    for (i = 9; i >= 0; i--) {
        Anim_Remove(q);
        q++;
    }
}

void SelectionPanel_HandleInput(struct SelectionPanel *panel)
{
    u16 *keys;
    s32 changed = 0;
    if (panel->maximumPage != 0) {
        if (gPadState[1] & 0x220) {
            panel->page--;
            if (panel->page < panel->minimumPage)
                panel->page = panel->maximumPage;
            changed = 1;
        } else if (gPadState[1] & 0x110) {
            panel->page++;
            if (panel->page > panel->maximumPage)
                panel->page = panel->minimumPage;
            changed = 1;
        }
        if (changed) {
            if (panel->page == 0) {
                panel->entryIds[1] = 0x1f;
                panel->maximumColumn = gUnk_0860654C[0];
            } else {
                panel->entryIds[1] = 0x20;
                panel->maximumColumn = gUnk_0860654C[1];
            }
            if (panel->column > panel->maximumColumn)
                panel->column = panel->maximumColumn;
            Anim_SelectWidgetGlyph(&panel->widgets[1], &panel->widgetResource, panel->entryIds[1]);
            RecordPanel_RefreshEntryLabels(panel);
        }
    }
    {
        if (gPadState[1] & 0x40) {
            panel->column--;
            if (panel->column < 0)
                panel->column = panel->maximumColumn;
            changed = 1;
        } else if (gPadState[1] & 0x80) {
            panel->column++;
            if (panel->column > panel->maximumColumn)
                panel->column = 0;
            changed = 1;
        }
        keys = gPadState;
        if (keys[0] & 0x40) {
            if (panel->heldKeys & 0x80)
                panel->repeat = 0;
            else
                panel->repeat++;
            panel->heldKeys = 0x40;
        } else if (keys[0] & 0x80) {
            if (panel->heldKeys & 0x40)
                panel->repeat = 0;
            else
                panel->repeat++;
            panel->heldKeys = 0x80;
        } else {
            panel->repeat = 0;
        }
        if (panel->repeat > 10) {
            if (keys[0] & 0x40) {
                panel->column--;
                if (panel->column < 0)
                    panel->column = panel->maximumColumn;
            } else if (keys[0] & 0x80) {
                panel->column++;
                if (panel->column > panel->maximumColumn)
                    panel->column = 0;
            }
            changed = 1;
        }
    }
    if (changed) {
        RecordPanel_DrawTime(panel);
        panel->repeat = 0;
        Sound_PlaySong(0xdc);
    }
    panel->widgets[3].position.y = panel->column << 4;
}

void SelectionPanel_InitializeTextState(struct SelectionPanel *panel)
{
    panel->lastDescription = 0xFF;
    ScrollingDialogue_ClearScript();
    ScrollingDialogue_RequestShow();
}

void SelectionPanel_UpdateDescription(struct SelectionPanel *panel)
{
    s32 selection = SelectionPanel_GetSelection(panel);
    if (selection < 0)
        selection = 0;
    if (panel->lastDescription != selection) {
        ScrollingDialogue_ClearScript();
        if (StateFlags_TestIndexedFlag(selection)) {
            ScrollingDialogue_SetScript(
                Text_LookupString(Script_ParseStringRef(panel->descriptionText) + selection));
        } else {
            ScrollingDialogue_SetScript(
                Text_LookupString(Script_ParseStringRef(panel->descriptionText) + 12));
        }
        panel->lastDescription = selection;
    }
}

s32 SelectionPanel_GetSelection(struct SelectionPanel *panel)
{
    s32 index;
    switch (panel->page) {
        default:
            return 0;
        case 0:
            index = 0;
            break;
        case 1:
            index = 6;
            break;
    }
    index += panel->column;
    return index;
}

void SelectionPanel_Close(struct SelectionPanel *panel)
{
    ScrollingDialogue_ClearScript();
    ScrollingDialogue_HideImmediately();
    Bg_ClearTilemap(0);
    Bg_ClearTilemap(1);
    Bg_ClearTilemap(2);
    Bg_ClearTilemap(3);
    SelectionPanel_HideDisplayObjects(panel);
    Registry1745_SetMenuRequest(1);
    Task_Kill(panel);
}

void SelectionPanel_CommitSelection(struct SelectionPanel *panel)
{
    unsigned int two;
    struct AreaEffectLinkState *context = gLinkSession;
    if (context && context->unavailable == 1) {
        if (&context->exchangeValue) {
            s8 *column;
            column = &panel->column;
            if (*column > 4) {
                u32 offset;
                gRandomIndex = (gRandomIndex + 1) & 1023;
                two = 2;
                offset = gRandomIndex * two;
                *column = Mod(*(const u16 *)(offset + (u32)((const u16 *)0x0203B400)), 5);
            }
            /* This protocol writes only the low byte of the shared exchange word. */
            *(u8 *)&context->exchangeValue = SelectionPanel_GetSelection(panel);
        }
    }
}

void SelectionPanel_RunSelectionScript(struct SelectionPanel *panel)
{
    s32 value;
    struct SelectionPanelScriptArgs args;
    ScrollingDialogue_ClearScript();
    ScrollingDialogue_HideImmediately();
    Bg_ClearTilemap(0);
    Bg_ClearTilemap(1);
    Bg_ClearTilemap(2);
    Bg_ClearTilemap(3);
    SelectionPanel_HideDisplayObjects(panel);
    if (panel->selectionScript) {
        value = SelectionPanel_GetSelection(panel);
        args.count = 1;
        args.values = &value;
        Script_Call(panel->selectionScript, &args);
        {
            void (*update)(struct SelectionPanel *) = SelectionPanel_OnTransitionIdle;
            u32 state = 3;
            panel->update = update;
            panel->entering = 1;
            panel->state = state;
        }
    }
}

void SelectionPanel_RunCancelScript(struct SelectionPanel *panel)
{
    ScrollingDialogue_ClearScript();
    ScrollingDialogue_HideImmediately();
    Bg_ClearTilemap(0);
    Bg_ClearTilemap(1);
    Bg_ClearTilemap(2);
    Bg_ClearTilemap(3);
    SelectionPanel_HideDisplayObjects(panel);
    if (panel->cancelScript) {
        Script_Call(panel->cancelScript, 0);
        {
            void (*update)(struct SelectionPanel *) = SelectionPanel_OnTransitionIdle;
            u32 state = 3;
            panel->update = update;
            panel->entering = 1;
            panel->state = state;
        }
    }
}

void SelectionPanel_OnCloseRequested(struct SelectionPanel *panel)
{
    u8 *flag = &panel->entering;

    if (*flag) {
        SelectionPanel_CommitSelection(panel);
        SelectionPanel_Close(panel);
        *flag = 0;
    }
}

void SelectionPanel_UpdateConfirmTransition(struct SelectionPanel *panel)
{
    u8 *flag = &panel->entering;
    if (*flag) {
        ScreenTransition_SendCommand(3, 4, 4, 4, 4, 0xffff, 0);
        GameState_SetResultIndex(panel->column + panel->page * 6);
        panel->timer = 0;
        *flag = 0;
    }
    CharacterModel_UpdatePairedAnimations(&panel->preview.model, 0);
    PaletteFade_CycleMenuColors(panel->paletteCycle.colors);
    Palette_UpdateHighlightPulse(&panel->widgetPalette[12], &panel->pulseFrame);
    if (panel->timer <= 31)
        ++panel->timer;
    else
        SelectionPanel_RunSelectionScript(panel);
}

void SelectionPanel_UpdateCancelTransition(struct SelectionPanel *panel)
{
    u8 *flag = &panel->entering;
    if (*flag) {
        ScreenTransition_SendCommand(3, 4, 4, 4, 4, 0xffff, 0);
        panel->timer = 0;
        *flag = 0;
    }
    CharacterModel_UpdatePairedAnimations(&panel->preview.model, 0);
    PaletteFade_CycleMenuColors(panel->paletteCycle.colors);
    Palette_UpdateHighlightPulse(&panel->widgetPalette[12], &panel->pulseFrame);
    if (panel->timer <= 31)
        ++panel->timer;
    else
        SelectionPanel_RunCancelScript(panel);
}

void SelectionPanel_OnTransitionIdle(void)
{
}

void SelectionPanel_UpdateInput(struct SelectionPanel *panel)
{
    void (*cb)(struct SelectionPanel *);
    u8 one;
    Refresh(panel);
    CharacterModel_UpdatePairedAnimations(&panel->preview.model, 0);
    PaletteFade_CycleMenuColors(panel->paletteCycle.colors);
    Palette_UpdateHighlightPulse(&panel->widgetPalette[12], &panel->pulseFrame);
    if (panel->timer < 30) {
        panel->timer++;
        return;
    }
    SelectionPanel_HandleInput(panel);
    SelectionPanel_UpdateDescription(panel);
    panel->previousSelection = panel->selection;
    if (gPadState[1] & 1) {
        if (TestSel(panel)) {
            Sound_PlaySong(0xdd);
            if (LinkActive()) {
                cb = SelectionPanel_OnCloseRequested;
                one = 1;
                panel->update = cb;
                panel->entering = one;
                panel->state = one;
            } else {
                cb = SelectionPanel_UpdateConfirmTransition;
                one = 1;
                panel->update = cb;
                panel->entering = one;
                panel->state = one;
            }
        } else {
            Sound_PlaySong(0x192);
        }
    } else if (gPadState[1] & 2) {
        if (!LinkActive()) {
            cb = SelectionPanel_UpdateCancelTransition;
            panel->update = cb;
            panel->entering = 1;
            panel->state = 2;
            Sound_PlaySong(0xde);
        }
    }
}

s32 SelectionPanel_Update(struct SelectionPanel *panel)
{
    u32 mask = 0x800;
    if ((gEngineFlags | gScenePauseFlags) & mask) {
        if (Registry1745_GetSessionState() == 0) {
            ScrollingDialogue_HideImmediately();
            Task_Kill(panel);
            return -1;
        }
    }
    if (panel->update != 0)
        panel->update(panel);
    return 1;
}

s32 SelectionPanel_OnDestroy(struct SelectionPanel *panel)
{
    SelectionPanel_ClearTextWindows(panel);
    SelectionPanel_RemoveWidgets(panel);
    CharacterModel_RemoveAnimations(&panel->preview.model);
    gConfirmPanel = 0;
    return 1;
}

s32 SelectionPanel_InitializeFromScript(struct SelectionPanel *panel)
{
    s32 zero;
    void (*cb)(struct SelectionPanel *);
    if (Script_SeekToKeyword(0x73)) {
        panel->sourceScript = Script_GetTextSourcePc();
        if (panel->sourceScript == 0)
            return -1;
    }
    if (Script_SeekToKeyword(0x63)) {
        panel->contextScript = Script_GetTextSourcePc();
        if (panel->contextScript == 0)
            return -1;
    }
    if (Script_SeekToKeyword(0x61)) {
        panel->descriptionText = Script_GetTextSourcePc();
        if (panel->descriptionText == 0)
            return -1;
    }
    if (Script_SeekToKeyword(0x6e)) {
        panel->nameScript = Script_GetPc();
        if (panel->nameScript == 0)
            return -1;
    }
    {
        u32 mask = 0x800;
        u32 m = (gEngineFlags | gScenePauseFlags) & mask;
        if (m) {
            panel->page = 1;
            panel->maximumPage = 0;
            panel->maximumColumn = 5;
            panel->repeat = 0;
            panel->heldKeys = 0;
            panel->column = 0;
        } else {
            panel->page = m;
            panel->maximumPage = 1;
            panel->maximumColumn = 5;
            panel->repeat = m;
            panel->heldKeys = m;
            panel->column = m;
        }
    }
    panel->selectionScript = Script_GetKeywordValue(0x65, 0);
    panel->cancelScript = Script_GetKeywordValue(0x43, 0);
    Bg_ClearTilemap(3);
    Bg_ClearTilemap(2);
    Bg_ClearTilemap(1);
    Bg_ClearTilemap(0);
    Registry3C9F_LoadBackground(panel);
    SelectionPanel_InitEntryIds(panel);
    SelectionPanel_InitializeWidgets(panel);
    SelectionPanel_InitPreview(panel);
    SelectionPanel_PositionWidgets(panel);
    ModifierMenu_InitSelectionSnapshot(&panel->paletteCycle, panel->paletteResource);
    sub_0810D760(panel);
    RecordPanel_ShowName(panel);
    SelectionPanel_InitializeTextState(panel);
    SelectionPanel_UpdateDescription(panel);
    cb = SelectionPanel_UpdateInput;
    zero = 0;
    panel->update = cb;
    panel->entering = 1;
    panel->state = zero;
    panel->widgets[9].flags |= 1;
    if (Script_GetKeywordValue(0x66, 0)) {
        ScreenTransition_SendCommand(2, 5, 4, 4, 4, 0xFFFF, zero);
    }
    gConfirmPanel = (u32)panel;
    return 1;
}

struct SelectionPanel *SelectionPanel_Create(void)
{
    struct SelectionPanel *panel;
    if (gConfirmPanel != 0)
        return (void *)gConfirmPanel;
    panel = Task_Create(0xb, 0xd38);
    if (panel != 0) {
        Task_SetCallbacks(panel, (void (*)(void *))SelectionPanel_Update,
                          (void (*)(void *))SelectionPanel_OnDestroy);
        if (SelectionPanel_InitializeFromScript(panel) < 0) {
            Task_Kill(panel);
            return 0;
        }
    }
    return panel;
}
