#include "challenge_menu.h"

s32 ChallengeMenu_GetStoredRank(s32 group, s32 choice)
{
    s32 index = group * 3 + choice;
    s32 byteIndex = index / 2;
    s32 odd = index & 1;
    u8 *packedRanks = (gGameState->unk654 + 4) + byteIndex;
    if (odd) {
        s32 value = *packedRanks;
        if (value >= 0)
            return value >> 4;
        return -((-value) >> 4);
    }
    return *packedRanks & 15;
}

void ChallengeMenu_SetStoredRank(s32 group, s32 choice, u32 rank)
{
    s32 index = group * 3 + choice;
    s32 byteIndex = index / 2;
    s32 odd = index & 1;
    u8 *packedRanks;
    u32 nibbleMask = 0xf;
    rank &= nibbleMask;
    packedRanks = (gGameState->unk654 + 4) + byteIndex;
    if (odd) {
        rank <<= 4;
        *packedRanks &= nibbleMask;
    } else {
        *packedRanks &= 0xf0;
    }
    *packedRanks += rank;
}

void ChallengeMenu_DrawOptions(struct ChallengeMenu *menu)
{
    s32 base;
    s32 i;
    s32 y;
    base = Script_ParseStringRef(menu->optionText);
    Menu_EraseRect(3, 6, 0x13, 2);
    Menu_DrawText(3, 6, Text_LookupString(base + 0xF));
    i = 0;
    y = 9;
    do {
        Menu_EraseRect(3, y, 0x13, 2);
        if ((menu->enabledChoices >> i) & 1)
            Menu_DrawText(3, y, Text_LookupString(base + i + menu->group * 3));
        else
            Menu_DrawText(3, y, Text_LookupString(base + 0x10));
        y += 2;
        i++;
    } while (i <= 2);
}

void ChallengeMenu_UpdateDescription(struct ChallengeMenu *menu)
{
    if (menu->selection == 0)
        goto close;
    if (menu->selection > 3)
        return;
    if (!((menu->enabledChoices >> (menu->selection - 1)) & 1)) {
    close:
        ScrollingDialogue_ClearScript();
        ScrollingDialogue_HideImmediately();
    } else {
        ScrollingDialogue_ClearScript();
        {
            s32 id = Script_ParseStringRef(menu->descriptionText) + menu->group * 3;
            id--;
            id += menu->selection;
            ScrollingDialogue_SetScript(Text_LookupString(id));
        }
        ScrollingDialogue_RequestShow();
    }
}

u8 ChallengeMenu_ConsumeStateEntry(struct ChallengeMenu *menu)
{
    u8 *q = &menu->stateEntered;
    s32 r;
    if (*q == 0) {
        r = 0;
    } else {
        *q = 0;
        r = 1;
    }
    return r;
}

void ChallengeMenu_SetState(struct ChallengeMenu *menu, void (*v)(struct ChallengeMenu *))
{
    menu->stateCallback = v;
    menu->stateTimer = 0;
    menu->stateEntered = 1;
}

void ChallengeMenu_StartFadeIn(struct ChallengeMenu *menu)
{
    ScreenTransition_SendCommand(0, 5, 4, 4, 4, 0xFFFF, 0);
    ChallengeMenu_SetState(menu, ChallengeMenu_UpdateFadeIn);
}

void ChallengeMenu_StartFadeOut(struct ChallengeMenu *menu)
{
    ScreenTransition_SendCommand(1, 5, 4, 4, 4, 0xFFFF, 0);
    ChallengeMenu_SetState(menu, ChallengeMenu_UpdateFadeOut);
}

static inline void HideWidget(struct SceneInputWidget *widget)
{
    widget->flags |= 1;
}

void ChallengeMenu_HideWidgets(struct ChallengeMenu *menu)
{
    s32 i;
    for (i = 0; i < 4; i++)
        HideWidget(&menu->widgets[i]);
    HideWidget(&menu->widgets[4]);
    HideWidget(&menu->widgets[5]);
}

void ChallengeMenu_HandleInput(struct ChallengeMenu *menu)
{
    u16 *keys = gPadState;
    u8 *timer;
    u32 repeatMask;
    u32 pendingKeys;
    if (keys[1] & 1) {
        if (((menu->enabledChoices >> (menu->selection - 1)) & 1) || menu->selection == 0) {
            ChallengeMenu_StartFadeOut(menu);
            Sound_PlaySong(0xDD);
            return;
        }
        Sound_PlaySong(0x192);
    } else if (keys[1] & 2) {
        menu->selection = 5;
        Sound_PlaySong(0xDE);
        ChallengeMenu_StartFadeOut(menu);
        return;
    }
    if (keys[0] & 0x40) {
        if (!(menu->previousKeys & 0x40))
            menu->repeatTimer = 0;
        timer = &menu->repeatTimer;
        if (++*timer > 9) {
            pendingKeys = keys[1];
            repeatMask = 0x40;
            goto repeat;
        }
    } else if (keys[0] & 0x80) {
        if (!(menu->previousKeys & 0x80))
            menu->repeatTimer = 0;
        timer = &menu->repeatTimer;
        if (++*timer > 9) {
            pendingKeys = keys[1];
            repeatMask = 0x80;
        repeat:
            pendingKeys = repeatMask | pendingKeys;
            repeatMask = 0;
            keys[1] = pendingKeys;
            *timer = repeatMask;
        }
    }
    menu->previousKeys = keys[0];
    if (keys[1] & 0x40) {
        Sound_PlaySong(0xDC);
        menu->selection = menu->selection > 0 ? menu->selection - 1 : 3;
        ChallengeMenu_UpdateDescription(menu);
    } else if (keys[1] & 0x80) {
        Sound_PlaySong(0xDC);
        menu->selection = menu->selection <= 2 ? menu->selection + 1 : 0;
        ChallengeMenu_UpdateDescription(menu);
    }
}

void ChallengeMenu_UpdateFadeIn(struct ChallengeMenu *menu)
{
    menu->stateTimer++;
    if (menu->stateTimer > 0x20)
        ChallengeMenu_SetState(menu, ChallengeMenu_UpdateSelection);
}

void ChallengeMenu_UpdateFadeOut(struct ChallengeMenu *menu)
{
    menu->stateTimer++;
    if (menu->stateTimer > 0x20) {
        ScrollingDialogue_ClearScript();
        ScrollingDialogue_HideImmediately();
        ChallengeMenu_SetState(menu, ChallengeMenu_DispatchSelection);
    }
}

void ChallengeMenu_DispatchSelection(struct ChallengeMenu *menu)
{
    if (ChallengeMenu_ConsumeStateEntry(menu) != 0) {
        u32 s = menu->selection;
        if (s == 5) {
            Script_Call(menu->cancelScript, 0);
            Task_Kill(menu);
        } else if (s <= 3) {
            u32 q = menu->selectionScripts[s];
            if (q != 0) {
                Script_Call(q, 0);
                Task_Kill(menu);
            }
        }
    }
}

void ChallengeMenu_UpdateSelection(struct ChallengeMenu *menu)
{
    if (ChallengeMenu_ConsumeStateEntry(menu))
        menu->repeatTimer = 0;
    ChallengeMenu_HandleInput(menu);
}

static inline void ShowWidget(struct SceneInputWidget *widget)
{
    widget->flags &= ~1;
}

s32 ChallengeMenu_UpdateWidgets(struct ChallengeMenu *menu)
{
    s32 *selection = &menu->selection;
    struct ContactPosition *position;
    s32 rank;

    if (*selection == 0) {
        Anim_LoadKeyframe(&menu->widgets[1], &menu->resources[1], 0x55, 0);
        position = &menu->widgets[1].position;
        position->x = 0;
        position->y = 0;
    } else if (*selection <= 3) {
        Anim_LoadKeyframe(&menu->widgets[1], &menu->resources[1], 0x56, 0);
        position = &menu->widgets[1].position;
        position->x = 0;
        position->y = (*selection - 1) << 4;
    }
    Palette_UpdateHighlightPulse(menu->highlightPalette, &menu->highlightPhase);
    ShowWidget(&menu->widgets[3]);
    if (menu->selection == 0) {
        HideWidget(&menu->widgets[2]);
    } else if (menu->selection <= 3) {
        rank = ChallengeMenu_GetRankKeyframe(
            ChallengeMenu_GetStoredRank(menu->group, menu->selection - 1));
        if (rank >= 0) {
            Anim_LoadKeyframe(&menu->widgets[2], &menu->resources[1], (u16)rank, 0);
            ShowWidget(&menu->widgets[2]);
        } else {
            HideWidget(&menu->widgets[2]);
        }
    }
}

void ChallengeMenu_EnableBackground(struct ChallengeMenu *menu)
{
    u32 m = 0x100;
    gDispCntShadow = m | gDispCntShadow;
    BgGfx_LoadTilemapAtPosition(1, 0, 0);
}

void ChallengeMenu_Update(struct ChallengeMenu *menu)
{
    ChallengeMenu_UpdateWidgets(menu);
    if (menu->stateCallback)
        Callback_InvokeActor(menu, menu->stateCallback);
}

void ChallengeMenu_Destroy(struct ChallengeMenu *menu)
{
    s32 i;
    struct SceneInputWidget *e = menu->widgets;
    for (i = 3; i >= 0; i--) {
        Anim_Remove(e);
        e++;
    }
    Anim_Remove(&menu->widgets[4]);
    Anim_Remove(&menu->widgets[5]);
    Bg_ClearTilemap(0);
    Bg_ClearTilemap(1);
    Bg_ClearTilemap(2);
    Bg_ClearTilemap(3);
}

void ChallengeMenu_InitBackgrounds(struct ChallengeMenu *menu)
{
    s32 value;
    s32 i;
    void *res;
    u16 *g = &gDispCntShadow;
    s32 mask = 0xFFFFF0FF;
    *g = mask & *g;
    sub_082157EC();
    Hud_SetMode(7);
    res = ScriptProc_Start(0xC091, 0x3536);
    menu->backgroundResource = res;
    value = 0x11;
    Bg_SetupLayers(3, 0, res, 0, 0, 1, &value);
    Bg_SetupLayers(2, 0, res, 0, 0, 1, &value);
    ChallengeMenu_EnableBackground(menu);
    i = 1;
    do {
        u16 *e = Video_GetWrappedBackgroundMapEntry(3, 1, i);
        s32 next = i + 1;
        s32 n = 4;
        do {
            *e++ = 0x2002;
        } while (--n >= 0);
        i = next;
    } while (i <= 4);
}

void ChallengeMenu_LoadPalette(struct ChallengeMenu *menu)
{
    struct ChallengeMenuPalette *resource = ScriptProc_Start(0x92B3, 0x204);
    u16 **destination = &menu->paletteSource;
    u16 *colors = resource->colors;
    *destination = colors;
    CpuSet(colors, gBgPaletteBuffer, 0x04000028);
}

void ChallengeMenu_SetGroupIcon(struct ChallengeMenu *menu)
{
    struct ChallengeMenuPositions positions;
    struct ChallengeMenuPositions keyframes;
    memcpy(&positions, &gUnk_0824F9C8, sizeof(positions));
    keyframes = gUnk_0824F9DC;
    menu->widgets[4].position.x = positions.entries[menu->group].x;
    {
        u32 entryOffset = menu->group << 2;
        u16 *firstY = &positions.entries[0].y;
        menu->widgets[4].position.y = *(u16 *)((u8 *)firstY + entryOffset);
    }
    Anim_LoadKeyframe(&menu->widgets[4], &menu->resources[2], keyframes.entries[menu->group].x, 0);
    menu->widgets[4].attribute1A = 3;
}

void ChallengeMenu_InitializeKeyframes(struct ChallengeMenu *menu)
{
    struct ChallengeMenuFrames frames = gUnk_0824F9F0;
    s32 i;
    struct SceneInputResource *source = &menu->resources[1];
    u32 *frame = frames.frames;
    struct SceneInputWidget *animation = menu->widgets;
    for (i = 0; i < 4; i++) {
        Anim_LoadKeyframe(animation, &menu->resources[1], (u16)*frame++, 0);
        animation++;
    }
    {
        u16 *position = (u16 *)&menu->widgets[2].position.x;
        u8 zero = 0;
        u32 v;
        *position = 0x92;
        *(u16 *)&menu->widgets[2].position.y = v = 0xFFE8;
        Anim_LoadKeyframe(&menu->widgets[0], source, (u16)(menu->group + 0x50), 0);
        *(u16 *)&menu->widgets[1].paletteBank = v = 0xD008;
        menu->widgets[1].palette = menu->palette;
        CpuSet(gPaletteBankData + 0x64E0, menu->palette, 0x04000008);
        menu->highlightPhase = zero;
    }
}

void ChallengeMenu_InitAnimations(struct ChallengeMenu *menu)
{
    u32 key = 0xCB05;
    struct SceneInputResource *resource = ScriptProc_Start(key, 0xA2DB);
    struct SceneInputResource *first, *last;
    u32 zero, duration, initialState;
    u16 bits;
    u16 *dst;
    s32 i;
    struct SceneInputWidget *entry;
    menu->resources[2] = *resource;
    first = &menu->resources[2];
    ResourceHeader_Relocate(first, resource);
    resource = ScriptProc_Start(key, 0x1289);
    menu->resources[1] = *resource;
    ResourceHeader_Relocate(&menu->resources[1], resource);
    resource = ScriptProc_Start(key, 0x530D);
    menu->resources[0] = *resource;
    last = &menu->resources[0];
    ResourceHeader_Relocate(last, resource);
    Widget_InitLinked(&menu->widgets[5], last, 0x8A, 0x10, zero = 0, 0, duration = 0x3C, 0);
    dst = (u16 *)&menu->widgets[5].position.y;
    bits = 0xFFE8;
    *dst = bits;
    Widget_InitLinked(&menu->widgets[4], first, 0, 0x10, 3, zero, duration, zero);
    initialState = 0;
    entry = &menu->widgets[0];
    i = 3;
    do {
        Widget_InitLinked(entry, &menu->resources[1], 0, 0x10, initialState, initialState, 0x3C,
                          initialState);
        entry++;
    } while (--i >= 0);
    ChallengeMenu_SetGroupIcon(menu);
    ChallengeMenu_InitializeKeyframes(menu);
}

s32 ChallengeMenu_ScriptOpen(struct ChallengeMenu *menu)
{
    if (Script_SeekToKeyword('e')) {
        u32 *out = menu->selectionScripts;
        s32 i = 3;
        do {
            *out++ = Script_GetValue();
        } while (--i >= 0);
    }
    menu->enabledChoices = Script_GetKeywordValue('o', 7);
    if (Script_SeekToKeyword('s')) {
        menu->group = Script_GetValue();
    }
    if (Script_SeekToKeyword('c')) {
        menu->cancelScript = Script_GetValue();
    } else {
        menu->cancelScript = 0;
    }
    if (Script_SeekToKeyword('m')) {
        menu->descriptionText = (const void *)Script_GetTextSourcePc();
    }
    if (Script_SeekToKeyword('n')) {
        menu->optionText = (const void *)Script_GetTextSourcePc();
    }
    if (Script_SeekToKeyword('A')) {
        s32 i = 0;
        s32 *base = &menu->group;
        for (; i <= 2; i++) {
            s32 value = Script_GetValue();
            ChallengeMenu_SetStoredRank(*(s32 *)base, i, value);
        }
    }
    menu->stateCallback = 0;
    menu->stateTimer = 0;
    ChallengeMenu_InitBackgrounds(menu);
    ChallengeMenu_LoadPalette(menu);
    ChallengeMenu_InitAnimations(menu);
    ChallengeMenu_UpdateDescription((struct ChallengeMenu *)menu);
    ChallengeMenu_DrawOptions(menu);
    ChallengeMenu_StartFadeIn(menu);
    menu->selection = 0;
    return 0;
}

struct ChallengeMenu *ChallengeMenu_CreateFromScript(void)
{
    struct ChallengeMenu *p = Task_Create(8, sizeof(*p));
    if (p != 0) {
        Task_SetCallbacks(p, (void (*)(void *))ChallengeMenu_Update,
                          (void (*)(void *))ChallengeMenu_Destroy);
        if (ChallengeMenu_ScriptOpen(p) < 0) {
            Task_Kill(p);
            return 0;
        }
    }
    return p;
}

void ChallengeMenu_ResetStoredRanks(void)
{
    *(s32 *)((gGameState->unk654 + 4)) = -1;
    *(s32 *)((gGameState->unk654 + 8)) = -1;
}

s32 ChallengeMenu_GetRankKeyframe(s32 n)
{
    s32 r;
    switch (n) {
        case 1:
            r = 0x26;
            break;
        case 2:
            r = 0x25;
            break;
        case 3:
            r = 0x24;
            break;
        case 4:
            r = 0x23;
            break;
        case 5:
            r = 0x22;
            break;
        default:
            r = -1;
            break;
    }
    return r;
}
