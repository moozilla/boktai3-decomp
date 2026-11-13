#include "link_record_panel.h"
#include "area_effect_link.h"
#include "scrolling_dialogue.h"
#include "palette_resource.h"

void TextWindow_StartPlayback(s32);

s32 TextWindow_Clear(s32);

void TextWindow_SetString(s32, s32, void *);

void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);

s32 TextWindow_Create(s32, s32, s32, s32);

u16 *Video_GetWrappedBackgroundMapEntry(s32, u8 *, u32);

void PanelText_DrawDigits(s32, s32, u8 *, u32, s32);

void Video_DrawBackgroundRowMarkers(u8 *, u32);

void ActorSprite_InitPartDrawings(void *, void *, u32);

void CharacterPreview_PositionParts(void *, void *, void *, s32);

void CharacterModel_SetAnimationFlags(void *, s32);

void CharacterModel_SetComponentRenderMode(void *, s32);

void CharacterModel_InitPalettes(void *, s32);

void CharacterModel_UpdatePairedAnimations(void *, s32);

void CharacterModel_RemoveAnimations(void *);

void Bg_SetLayerMap(u32, void *, u16);

void PaletteFade_CycleMenuColors(void *);

s32 Registry1745_GetSessionState(void);

void Registry1745_SetMenuRequest(u32);

void ModifierMenu_InitSelectionSnapshot(void *, void *);

void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);

static inline s32 clampIndex(s32 index, s32 low, s32 high)
{
    s32 result = index;
    if (index > high)
        result = high;
    else if (index < low)
        result = low;
    return result;
}

static inline struct LinkTimeRecord *getPeer(s32 index)
{
    struct AreaEffectLinkState *session = gLinkSession;
    struct LinkRecordPayload *payload;
    if (!session || session->unavailable != 1)
        return NULL;
    payload = (struct LinkRecordPayload *)session->sessionData;
    return &payload->peers[index];
}

static inline u8 validSelection(s32 index)
{
    if (index < 0)
        goto no;
    if (index <= 1)
        goto yes;
no:
    return 0;
yes:
    return 1;
}

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

static inline void PanelSequence_SetUpdate(struct LinkRecordPanel *panel,
                                           void (*update)(struct LinkRecordPanel *))
{
    panel->update = update;
}

static inline u8 PanelSequence_ClampSelectionCount(u8 value, s32 min, s32 max)
{
    if (value > max)
        value = max;
    else if (value < min)
        value = min;
    return value;
}

void LinkRecordPanel_LoadBackground(struct LinkRecordPanel *owner)
{
    u32 value;
    void *resource;
    void *palette;
    resource = ScriptProc_Start(0xC091, 0xCE15);
    owner->backgroundResource = resource;
    value = 2;
    Bg_SetupLayers(2, 0, resource, 0, 0, 1, &value);
    palette = ((struct PaletteResource *)ScriptProc_Start(0x92B3, 0xA2F6))->banks;
    owner->paletteResource = palette;
    CpuSet(palette, gBgPaletteBuffer, 0x100);
}

void LinkRecordPanel_DrawTime(struct LinkTimeRecord *record, u8 *text, u32 style, s32 index)
{
    s32 value, whole, fraction;
    if (!record)
        Video_DrawBackgroundRowMarkers(text, style);
    value = record->times[index];
    if (value < 0) {
        Video_DrawBackgroundRowMarkers(text, style);
        return;
    }
    if (value > 59999) {
        fraction = 999;
        whole = fraction;
    } else {
        whole = Div(value, 60);
        fraction = Div((value - whole * 60) * 169, 10);
    }
    PanelText_DrawDigits(whole, 100, text, style, 0);
    PanelText_DrawDigits(fraction, 100, text + 4, style, 0);
    *Video_GetWrappedBackgroundMapEntry(3, text + 7, style) = 1;
}

void LinkRecordPanel_DrawPeerTimes(struct LinkRecordPanel *owner)
{
    struct LinkTimeRecord *peer = getPeer(0);
    s32 index = owner->selection - 6;
    index = clampIndex(index, 0, 5);
    LinkRecordPanel_DrawTime(peer, (u8 *)2, 15, index);
    peer = getPeer(1);
    LinkRecordPanel_DrawTime(peer, (u8 *)20, 15, index);
}

void LinkRecordPanel_CreateTextWindows(struct LinkRecordPanel *owner)
{
    s32 index;
    owner->titleWindow = TextWindow_Create(1, 3, 28, 2);
    owner->leftWindow = TextWindow_Create(1, 6, 10, 2);
    owner->rightWindow = TextWindow_Create(19, 6, 10, 2);
    index = owner->selection - 5;
    index = clampIndex(index, 0, 5);
    TextWindow_SetScript(owner->titleWindow, owner->contextScript);
    TextWindow_Refresh(owner->titleWindow, index);
    TextWindow_StartPlayback(owner->titleWindow);
}

void LinkRecordPanel_StartDescriptions(struct LinkRecordPanel *owner)
{
    struct AreaEffectLinkState *sel = gLinkSession;
    if (sel != 0 && sel->unavailable == 1 &&
        (void *)&((struct LinkRecordPayload *)sel->sessionData)->peers[0] != 0) {
        s32 *w = &owner->leftWindow;
        TextWindow_SetScript(*w, owner->namesScript);
        TextWindow_Refresh(*w, 0);
        TextWindow_SetString(*w, 1,
                             ((struct LinkRecordPayload *)sel->sessionData)->peers[0].displayName);
        TextWindow_StartPlayback(*w);
        sel = gLinkSession;
        if (sel != 0 && sel->unavailable == 1 &&
            (void *)&((struct LinkRecordPayload *)sel->sessionData)->peers[1] != 0) {
            w = &owner->rightWindow;
            TextWindow_SetScript(*w, owner->namesScript);
            TextWindow_Refresh(*w, 1);
            TextWindow_SetString(
                *w, 2, ((struct LinkRecordPayload *)sel->sessionData)->peers[1].displayName);
            TextWindow_StartPlayback(*w);
        }
    }
}

void LinkRecordPanel_ClearTextWindows(struct LinkRecordPanel *owner)
{
    TextWindow_Clear(owner->titleWindow);
    TextWindow_Clear(owner->leftWindow);
    TextWindow_Clear(owner->rightWindow);
}

void LinkRecordPanel_InitializeEntryValues(struct LinkRecordPanel *owner)
{
    owner->entryValues[0] = 0x2D;
    owner->entryValues[1] = 0x2E;
    owner->entryValues[2] = 0x27;
    owner->entryValues[3] = 0x28;
    owner->entryValues[4] = 0x29;
    owner->entryValues[5] = 0x2F;
    owner->entryValues[6] = 0x2F;
}

void LinkRecordPanel_InitializeWidgets(struct LinkRecordPanel *owner)
{
    struct SceneInputWidget *sprite;
    int i;
    void *script;
    int zero;
    script = ScriptProc_Start(0xCB05, 0x1289);
    owner->widgetScript = script;
    ResourceHeader_Relocate(&owner->widgetResource, script);
    i = 0;
    zero = 0;
    sprite = owner->widgets;
    do {
        Widget_InitLinked(sprite, &owner->widgetResource, owner->entryValues[i], 0x10, zero, zero,
                          zero, (void *)zero);
        owner->previousValues[i] = 0xFFFF;
        sprite++;
        i++;
    } while (i < 7);
}

void LinkRecordPanel_PositionWidgets(struct LinkRecordPanel *owner)
{
    s32 i;
    for (i = 0; i <= 6; i++) {
        switch (i) {
            case 5:
                owner->widgets[i].position.x = 0;
                owner->widgets[i].position.y = 0;
                break;
            case 6:
                owner->widgets[i].position.x = 0x90;
                owner->widgets[i].position.y = 0;
                break;
            default:
                owner->widgets[i].position.x = 0;
                owner->widgets[i].position.y = 0;
                break;
        }
    }
}

void LinkRecordPanel_InitializePreviews(struct LinkRecordPanel *panel)
{
    s32 i;
    struct SceneInputPackedPosition pos;
    for (i = 0; i <= 1; i++) {
        struct AreaEffectLinkState *link = gLinkSession;
        struct LinkRecordIncomingStyle *payload;
        struct LinkRecordPreview *preview;
        struct CharacterPreviewModel *model;
        s32 flags;
        s32 k;
        if (!link || link->unavailable != 1 || !validSelection(i))
            return;
        payload = &((struct LinkRecordIncomingStyle *)link->incomingPayload)[i];
        if (!payload)
            return;
        preview = &panel->previews[i];
        preview->kind = payload->kind;
        preview->form = payload->form;
        preview->variant = payload->variant;
        preview->extra = payload->extra;
        preview->palette = payload->palette;
        preview->player = i;
        switch (i) {
            case 0:
                flags = 0;
                pos.x = 0x30;
                pos.y = 0x68;
                pos.z = 0;
                break;
            case 1:
                flags = 4;
                pos.x = 0xC0;
                pos.y = 0x68;
                pos.z = 0;
                break;
            default:
                flags = 0;
                Memory_Clear(&pos, 8);
                break;
        }
        model = &preview->model;
        ActorSprite_InitPartDrawings(preview, model, 0x3D99);
        CharacterPreview_PositionParts(preview, model, &pos, flags);
        CharacterModel_SetAnimationFlags(model, flags);
        CharacterModel_SetComponentRenderMode(model, 0);
        CharacterModel_InitPalettes(model, preview->palette);
        for (k = 0; k <= 4; k++)
            preview->model.view.parts[k].tile = i * 2 - 0x6000;
    }
}

void LinkRecordPanel_HideDisplay(struct LinkRecordPanel *owner)
{
    u32 flag = 1;
    u32 *q = &owner->widgets[0].flags;
    struct CharacterPreviewModel *r;
    s32 i;

    for (i = 6; i >= 0; i--) {
        *q |= flag;
        q += sizeof(owner->widgets[0]) / sizeof(*q);
    }
    r = &owner->previews[0].model;
    for (i = 1; i >= 0; i--) {
        CharacterModel_SetAnimationFlags(r, 1);
        r = (struct CharacterPreviewModel *)((u8 *)r + sizeof(owner->previews[0]));
    }
}

void LinkRecordPanel_RemoveWidgets(struct LinkRecordPanel *owner)
{
    struct SceneInputWidget *q = owner->widgets;
    s32 i;

    for (i = 6; i >= 0; i--) {
        Anim_Remove(q);
        q++;
    }
}

void LinkRecordPanel_ShowDialogue(void)
{
    ScrollingDialogue_ClearScript();
    ScrollingDialogue_RequestShow();
}

void LinkRecordPanel_OnIdle(void)
{
}

void LinkRecordPanel_RunExitScript(struct LinkRecordPanel *self)
{
    ScrollingDialogue_ClearScript();
    ScrollingDialogue_HideImmediately();
    Bg_ClearTilemap(0);
    Bg_ClearTilemap(1);
    Bg_ClearTilemap(2);
    Bg_ClearTilemap(3);
    LinkRecordPanel_HideDisplay(self);
    Registry1745_SetMenuRequest(2);
    if (self->exitScript != 0) {
        Script_Call(self->exitScript, 0);
        Task_Kill(self);
    }
}

void LinkRecordPanel_RunCompletionScript(struct LinkRecordPanel *self)
{
    if (self->completionScript != 0) {
        Script_Call(self->completionScript, 0);
        Task_Kill(self);
    }
}

void LinkRecordPanel_UpdateExit(struct LinkRecordPanel *owner)
{
    if (owner->entering) {
        ScreenTransition_SendCommand(3, 4, 4, 4, 4, 0xFFFF, 0);
        owner->timer = 0;
        owner->entering = 0;
    }
    if (owner->timer > 15) {
        Registry1745_SetMenuRequest(1);
        Task_Kill(owner);
    } else {
        owner->timer++;
    }
}

void LinkRecordPanel_UpdateDelay(struct LinkRecordPanel *owner)
{
    int i;
    struct CharacterPreviewModel *panel;
    if (owner->entering) {
        Bg_SetLayerMap(3, owner->backgroundResource, 2);
        BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
        LinkRecordPanel_DrawPeerTimes(owner);
        owner->timer = 0;
        owner->entering = 0;
    }
    i = 0;
    panel = &owner->previews[0].model;
    do {
        CharacterModel_UpdatePairedAnimations(panel, i);
        panel = (struct CharacterPreviewModel *)((u8 *)panel + sizeof(owner->previews[0]));
        i++;
    } while (i < 2);
    PaletteFade_CycleMenuColors(&owner->paletteCycle);
    if (owner->timer < 240) {
        owner->timer++;
    } else {
        void (*next)(struct LinkRecordPanel *) = LinkRecordPanel_UpdateExit;
        u32 one = 1;
        owner->update = next;
        owner->entering = one;
        owner->transition = one;
    }
}

s32 LinkRecordPanel_Update(struct LinkRecordPanel *owner)
{
    if (Registry1745_GetSessionState() == 0) {
        ScrollingDialogue_HideImmediately();
        Task_Kill(owner);
        return -1;
    }
    if (owner->update != 0)
        owner->update(owner);
    return 1;
}

s32 LinkRecordPanel_Destroy(struct LinkRecordPanel *owner)
{
    s32 i;
    u32 z;
    struct CharacterPreviewModel *preview;
    LinkRecordPanel_RemoveWidgets(owner);
    preview = &owner->previews[0].model;
    i = 1;
    do {
        CharacterModel_RemoveAnimations(preview);
        preview = (struct CharacterPreviewModel *)((u8 *)preview + sizeof(owner->previews[0]));
        i--;
    } while (i >= 0);
    z = 0;
    gPanelSequence = z;
    return 1;
}

s32 LinkRecordPanel_InitializeFromScript(struct LinkRecordPanel *owner)
{
    u8 *count;
    s32 selection;

    if (Script_SeekToKeyword('s')) {
        owner->sourceScript = (u32)Script_GetTextSourcePc();
        if (!owner->sourceScript)
            return -1;
    }
    if (Script_SeekToKeyword('c')) {
        owner->contextScript = (const u8 *)Script_GetTextSourcePc();
        if (!owner->contextScript)
            return -1;
    }
    if (Script_SeekToKeyword('n')) {
        owner->namesScript = Script_GetPc();
        if (!owner->namesScript)
            return -1;
    }
    owner->exitScript = Script_GetKeywordValue('e', 0);
    count = GetSelectionCount();
    if (!count)
        return -1;
    owner->selection = *count;
    selection = PanelSequence_ClampSelectionCount(owner->selection, 0, 10);
    {
        owner->selection = selection;
    }
    Bg_ClearTilemap(3);
    Bg_ClearTilemap(2);
    Bg_ClearTilemap(1);
    Bg_ClearTilemap(0);
    LinkRecordPanel_LoadBackground(owner);
    LinkRecordPanel_InitializeEntryValues(owner);
    LinkRecordPanel_InitializeWidgets(owner);
    LinkRecordPanel_InitializePreviews(owner);
    LinkRecordPanel_PositionWidgets(owner);
    ModifierMenu_InitSelectionSnapshot(&owner->paletteCycle, owner->paletteResource);
    LinkRecordPanel_CreateTextWindows(owner);
    LinkRecordPanel_StartDescriptions(owner);
    PanelSequence_SetUpdate(owner, LinkRecordPanel_UpdateDelay);
    owner->entering = 1;
    owner->transition = 0;
    if (Script_GetKeywordValue('f', 0))
        ScreenTransition_SendCommand(2, 5, 4, 4, 4, 0xFFFF, 0);
    gPanelSequence = (u32)owner;
    return 1;
}

struct LinkRecordPanel *LinkRecordPanel_Create(void)
{
    struct LinkRecordPanel *owner;
    if (gPanelSequence != 0)
        return (void *)gPanelSequence;
    owner = Task_Create(0xb, 0x1414);
    if (owner != 0) {
        Task_SetCallbacks(owner, (void (*)(void *))LinkRecordPanel_Update,
                          (void (*)(void *))LinkRecordPanel_Destroy);
        if (LinkRecordPanel_InitializeFromScript(owner) < 0) {
            Task_Kill(owner);
            return 0;
        }
    }
    return owner;
}
