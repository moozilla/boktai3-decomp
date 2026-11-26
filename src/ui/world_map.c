#include "ui/world_map.h"

void Palette_InterpolateBanks(void *, void *, void *, int, int);

void sub_082157EC(void);

void Hud_SetMode(s32);

u8 *sub_08216E54(u16);

void Bg_SetupLayers(int, int, void *, int, int, int, void *);

void Bg_SetPriority(u32, s32);

void BgGfx_LoadTilemapAtPosition(s32, s32, s32);

void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);

void Video_SetRenderCallbacks(s32, void *, void *, void *);

void SpriteOam_QueueProjectedSprites(void);

void Sprite_RenderScaledList(void);

void sub_08219624(void);

s32 SunClock_GetPhase(void);

extern const struct MenuCursorEntry gUnk_0824F7E8[9];

void *memcpy(void *, const void *, u32);

static inline u32 Test(struct MenuCursor *cursor, u32 mask)
{
    return cursor->stateFlags & mask;
}

static inline u32 Entry_TestFlags(struct MenuCursorEntry *entry, u32 mask)
{
    return entry->bounds.flags & mask;
}

void PositionMotion_InitTimedInterpolation(struct PositionMotion *motion,
                                           struct ContactPosition *position,
                                           struct ContactPosition *from, struct ContactPosition *to,
                                           u16 duration)
{
    motion->position = position;
    motion->from = *from;
    motion->to = *to;
    *position = *from;
    motion->mode = 0xc;
    motion->duration = duration;
    motion->timer = 0;
    motion->active = 1;
}

void WorldMap_SetUpdate(struct RegistrySelectionMenu *menu,
                                 void (*update)(struct RegistrySelectionMenu *))
{
    menu->update = update;
    menu->timer = 0;
}

void WorldMap_ShowIndicators(struct RegistrySelectionMenu *menu)
{
    menu->animations[1].flags &= ~1;
    AnimSprite_Show(&menu->animations[2]);
    AnimSprite_Show(&menu->animations[3]);
    AnimSprite_Show(&menu->animations[4]);
    AnimSprite_Show(&menu->animations[5]);
    AnimSprite_Show(&menu->animations[8]);
    menu->animations[0].flags &= ~1;
    AnimSprite_Show(&menu->animations[10]);
    AnimSprite_Show(&menu->animations[9]);
    AnimSprite_Show(&menu->animations[11]);
}

void WorldMap_UpdateIndicators(struct RegistrySelectionMenu *menu)
{
    u32 available = menu->availableEntries;
    struct AnimSpriteResource *resource = &menu->resource;
    struct AnimSprite *widgets = menu->animations;
    struct AnimSprite *widget;
    struct MenuCursorEntry *entry;
    if (available & 2)
        Anim_LoadKeyframe(widgets + 1, resource, 14, 0);
    else
        Anim_LoadKeyframe(widgets + 1, resource, 13, 0);
    if (available & 4)
        Anim_LoadKeyframe(widgets + 2, resource, 16, 0);
    else
        Anim_LoadKeyframe(widgets + 2, resource, 15, 0);
    if (available & 8)
        Anim_LoadKeyframe(widgets + 3, resource, 18, 0);
    else
        Anim_LoadKeyframe(widgets + 3, resource, 17, 0);
    if (available & 16)
        Anim_LoadKeyframe(widgets + 4, resource, 20, 0);
    else
        Anim_LoadKeyframe(widgets + 4, resource, 19, 0);
    if (available & 32)
        Anim_LoadKeyframe(widgets + 5, resource, 22, 0);
    else
        Anim_LoadKeyframe(widgets + 5, resource, 21, 0);
    if (available & 64) {
        widget = widgets + 6;
        Anim_LoadKeyframe(widget, resource, 23, 0);
        widget->flags &= ~1;
    } else
        (widgets + 6)->flags |= 1;
    if (available & 128) {
        widget = widgets + 7;
        Anim_LoadKeyframe(widget, resource, 24, 0);
        widget->flags &= ~1;
    } else
        (widgets + 7)->flags |= 1;
    if (available & 256) {
        widget = widgets + 8;
        Anim_LoadKeyframe(widget, resource, 26, 0);
        widget->flags &= ~1;
    } else
        (widgets + 8)->flags |= 1;
    if (available & 1)
        Anim_LoadKeyframe(widgets, resource, 28, 0);
    else
        Anim_LoadKeyframe(widgets, resource, 27, 0);
    if (menu->selection >= 0 && Test(menu->selectionCursor, 2)) {
        (widgets + 9)->flags &= ~1;
        Anim_LoadKeyframe(widgets + 9, resource, (u16)(menu->selection + 29), 0);
    } else
        (widgets + 9)->flags |= 1;
    if (menu->update == WorldMap_UpdateTimedEffect || !menu->update) {
        (&menu->animations[11])->flags |= 1;
        (&menu->animations[12])->flags |= 1;
    } else {
        widgets = &menu->animations[11];
        entry = MenuCursor_GetEntry(menu->selectionCursor, menu->initialSelection);
        if (entry) {
            widgets->width = entry->x;
            widgets->height = entry->y;
            widgets->flags &= ~1;
        } else {
            widgets->width = 0;
            widgets->height = 0;
            widgets->flags |= 1;
        }
        if ((u16)menu->cancelSelection <= 8) {
            widgets = &menu->animations[12];
            entry = MenuCursor_GetEntry(menu->selectionCursor, menu->cancelSelection);
            if (entry) {
                widgets->width = entry->x + 16;
                widgets->height = entry->y;
                widgets->flags &= ~1;
            } else {
                widgets->width = 0;
                widgets->height = 0;
                widgets->flags |= 1;
            }
        } else
            (&menu->animations[12])->flags |= 1;
    }
}

void WorldMap_BlendPaletteBanks(u8 *output, u8 *from, u8 *to, int amount, int shift,
                                         int bankCount)
{
    u16(*outputBank)[16] = (u16(*)[16])output;
    u16(*firstBank)[16] = (u16(*)[16])from;
    u16(*secondBank)[16] = (u16(*)[16])to;
    int bank;
    for (bank = 0; bank < bankCount; bank++) {
        Palette_InterpolateBanks(outputBank[bank], firstBank[bank], secondBank[bank], amount,
                                 shift);
    }
}

void WorldMap_ApplyPalettePhase(struct RegistrySelectionMenu *menu, u32 phase)
{
    u8 *base = menu->palettes;
    u8 *extra;
    u32 flags = menu->availableEntries;
    if (flags & 0x80)
        extra = base + 0x120;
    else
        extra = 0;
    switch (phase) {
        case 0:
            WorldMap_BlendPaletteBanks(((u8 *)gBgPaletteBuffer), base, base + 0xc0, 0x30,
                                                6, 3);
            if (extra)
                WorldMap_BlendPaletteBanks(((u8 *)gBgPaletteBuffer) + 0x40, base + 0x40,
                                                    extra, 0x30, 6, 1);
            break;
        case 1:
            WorldMap_BlendPaletteBanks(((u8 *)gBgPaletteBuffer), base, base + 0xc0, 0x10,
                                                6, 3);
            if (extra)
                WorldMap_BlendPaletteBanks(((u8 *)gBgPaletteBuffer) + 0x40, base + 0x40,
                                                    extra, 0x10, 6, 1);
            break;
        case 2:
            CpuSet(base, ((u8 *)gBgPaletteBuffer), 0x04000018);
            break;
        case 3:
            WorldMap_BlendPaletteBanks(((u8 *)gBgPaletteBuffer), base, base + 0x60, 0x10,
                                                6, 3);
            break;
        case 4:
            WorldMap_BlendPaletteBanks(((u8 *)gBgPaletteBuffer), base + 0x60, base + 0xc0,
                                                0x30, 6, 3);
            if (extra)
                WorldMap_BlendPaletteBanks(((u8 *)gBgPaletteBuffer) + 0x40, base + 0xa0,
                                                    extra, 0x30, 6, 1);
            break;
        case 5:
            CpuSet(base + 0xc0, ((u8 *)gBgPaletteBuffer), 0x04000018);
            if (extra)
                CpuSet(base + 0x120, ((u8 *)gBgPaletteBuffer) + 0x40, 0x04000008);
            break;
    }
}

void WorldMap_BlendTimedPalettes(struct RegistrySelectionMenu *menu)
{
    u8 *base = menu->palettes;
    u8 *extra;
    u32 flags = menu->availableEntries;
    if (flags & 0x80)
        extra = base + 0x120;
    else
        extra = 0;
    switch (menu->mode) {
        case 0:
        case 2:
            if (!extra)
                extra = base + 0x100;
            WorldMap_BlendPaletteBanks(((u8 *)gBgPaletteBuffer), base + 0x60, base + 0xc0,
                                                menu->elapsed, menu->shift, 2);
            WorldMap_BlendPaletteBanks(((u8 *)gBgPaletteBuffer) + 0x40, base + 0xa0, extra,
                                                menu->elapsed, menu->shift, 1);
            break;
        case 1:
        case 3:
            if (!extra)
                extra = base + 0x100;
            WorldMap_BlendPaletteBanks(((u8 *)gBgPaletteBuffer), base + 0xc0, base,
                                                menu->elapsed, menu->shift, 2);
            WorldMap_BlendPaletteBanks(((u8 *)gBgPaletteBuffer) + 0x40, extra, base + 0x40,
                                                menu->elapsed, menu->shift, 1);
            break;
    }
}

void WorldMap_ProcessConfirm(struct RegistrySelectionMenu *menu)
{
    if (gPadState[1] & 1) {
        struct MenuCursor *list = menu->selectionCursor;
        u32 mask = 4;
        u32 flags = list->stateFlags;
        if (flags & mask)
            Sound_PlaySong(0x192);
        else if ((flags & 2) && menu->selection >= 0) {
            if (menu->entryValues[menu->selection])
                WorldMap_BeginFadeOut(menu);
            MenuCursor_SetSelectionFlag(menu->selectionCursor);
        }
    }
}

void WorldMap_OnUnusedTransition(void)
{
}

void WorldMap_InitializeAnimations(struct RegistrySelectionMenu *menu)
{
    struct AnimSpriteResource *resource = ScriptProc_Start(0xcb05, 0xa2db);
    if (resource) {
        struct AnimSprite *anim;
        void *palette;
        s32 count;
        u32 zero;
        menu->resource = *resource;
        ResourceHeader_Relocate(&menu->resource, resource);
        anim = menu->animations;
        palette = gPaletteBankData + 0x63c0;
        zero = 0;
        count = 12;
        do {
            AnimSprite_InitLinked(anim, &menu->resource, 0, 0x10, 2, zero, 0x3c, (void *)zero);
            anim->palette = palette;
            anim->attribute1A = 3;
            anim++;
        } while (--count >= 0);
        Anim_LoadKeyframe(&menu->animations[10], &menu->resource, 12, 0);
        menu->animations[10].attribute1A = 1;
        Anim_LoadKeyframe(&menu->animations[9], &menu->resource, 30, 0);
        menu->animations[9].attribute1A = 2;
        Anim_LoadKeyframe(&menu->animations[11], &menu->resource, 0, 0);
        menu->animations[11].attribute1A = 2;
        Anim_LoadKeyframe(&menu->animations[12], &menu->resource, 1, 0);
        menu->animations[12].attribute1A = 2;
    }
}

void WorldMap_InitializeBackground(struct RegistrySelectionMenu *menu)
{
    u8 *background;
    s32 config[2];
    u16 *display = &gDispCntShadow;
    s32 clearMask = ~0xf00;
    *display = clearMask & *display;
    sub_082157EC();
    Hud_SetMode(7);
    {
        u32 enabledLayers = 0xd00;
        *display = enabledLayers | *display;
    }
    background = sub_08216E54(0xdcf7);
    config[0] = 1;
    config[1] = 0;
    Bg_SetupLayers(1, 0, background, 0, 0, 2, config);
    Bg_SetPriority(3, 3);
    Bg_SetPriority(2, 2);
    menu->palettes = (u8 *)ScriptProc_Start(0x92b3, 0x7e1e) + 0x14;
    CpuSet(menu->palettes, gBgPaletteBuffer, 0x04000080);
}

void WorldMap_ConfigureDisplay(void)
{
    BgGfx_LoadTilemapAtPosition(3, 8, 0x48);
    BgGfx_LoadTilemapAtPosition(2, 0, 0);
    BgGfx_LoadTilemapAtPosition(0, 0, 0);
    Hud_SetMode(7);
}

void WorldMap_BeginFadeIn(struct RegistrySelectionMenu *menu)
{
    ScreenTransition_SendCommand(2, 5, 4, 4, 4, 0xFFFF, 0);
    WorldMap_SetUpdate(menu, WorldMap_UpdateOpeningDelay);
}

void WorldMap_BeginFadeOut(struct RegistrySelectionMenu *menu)
{
    ScreenTransition_SendCommand(3, 5, 4, 4, 4, 0xFFFF, 0xFFFF);
    WorldMap_SetUpdate(menu, WorldMap_UpdateScriptDelay);
}

void WorldMap_BeginSelection(struct RegistrySelectionMenu *menu)
{
    if (menu->inputDisabled == 0)
        MenuCursor_EnableInput(menu->selectionCursor);
    WorldMap_SetUpdate(menu, WorldMap_UpdateSelection);
}

void WorldMap_StartTimedEffect(struct RegistrySelectionMenu *menu, s32 shift, s32 mode)
{
    u32 one;
    if (shift > 0)
        menu->shift = shift;
    else
        menu->shift = 7;
    menu->elapsed = 0;
    menu->mode = mode;
    menu->inputDisabled = one = 1;
    MenuCursor_Hide(menu->selectionCursor);
    AnimSprite_Hide(&menu->animations[11]);
    AnimSprite_Hide(&menu->animations[12]);
    WorldMap_SetUpdate(menu, WorldMap_UpdateTimedEffect);
}

void WorldMap_UpdateTimedEffect(struct RegistrySelectionMenu *menu)
{
    WorldMap_BlendTimedPalettes(menu);
    menu->elapsed++;
    if (menu->elapsed > (1 << menu->shift))
        WorldMap_SetUpdate(menu, 0);
}

void WorldMap_UpdateSelection(struct RegistrySelectionMenu *menu)
{
    s32 v;
    if (menu->selectionCursor == 0)
        v = -1;
    else
        v = menu->selectionCursor->selectedIndex;
    menu->selection = v;
    WorldMap_ProcessConfirm(menu);
}

void WorldMap_UpdateOpeningDelay(struct RegistrySelectionMenu *menu)
{
    s32 v;
    if (menu->selectionCursor == 0)
        v = -1;
    else
        v = menu->selectionCursor->selectedIndex;
    menu->selection = v;
    menu->timer++;
    if (menu->timer > 0x3c)
        WorldMap_BeginSelection(menu);
}

void WorldMap_UpdateScriptDelay(struct RegistrySelectionMenu *menu)
{
    menu->timer++;
    if (menu->timer > 0x3c)
        Script_Call(menu->entryValues[menu->selection], 0);
}

void WorldMap_OnUpdate(struct RegistrySelectionMenu *menu)
{
    WorldMap_ConfigureDisplay();
    WorldMap_UpdateIndicators(menu);
    if (menu->update)
        Callback_InvokeActor(menu, menu->update);
}

void WorldMap_Destroy(struct RegistrySelectionMenu *menu)
{
    struct AnimSprite *animation = menu->animations;
    s32 i = 12;
    do {
        if (animation->active)
            Anim_Remove(animation);
        ++animation;
    } while (--i >= 0);
    if (menu->selectionCursor)
        menu->selectionCursor = 0;
    gSelectionPanel = 0;
}

void WorldMap_CreateSelectionCursor(struct RegistrySelectionMenu *menu, s32 selection)
{
    struct MenuCursorEntry entries[9];
    s32 i;
    s32 available;
    struct MenuCursorEntry *entry8, *entry6, *entry7;
    memcpy(entries, gUnk_0824F7E8, sizeof(entries));
    i = 0;
    entry8 = &entries[8];
    entry6 = &entries[6];
    entry7 = &entries[7];
    available = menu->availableEntries;
    for (; i < 9; i++) {
        if ((available >> i) & 1)
            entries[i].bounds.flags = 0;
        else if ((u32)(i - 6) <= 2)
            entries[i].bounds.flags |= 3;
        else
            entries[i].bounds.flags |= 2;
    }
    if (!Entry_TestFlags(entry8, 1)) {
        entries[1].bounds.previous = 8;
        entries[5].bounds.next = 8;
    }
    if (!Entry_TestFlags(entry6, 1)) {
        entries[2].bounds.next = 6;
        if (!Entry_TestFlags(entry7, 1))
            entries[6].bounds.next = 7;
        else
            entries[3].bounds.previous = 6;
    }
    if (!Entry_TestFlags(entry7, 1)) {
        entries[3].bounds.previous = 7;
        if (!Entry_TestFlags(entry6, 1))
            entries[7].bounds.previous = 6;
        else
            entries[2].bounds.next = 7;
    }
    menu->selectionCursor = MenuCursor_Create(1, selection, 9, entries);
}

s32 WorldMap_InitializeFromScript(struct RegistrySelectionMenu *menu, void *arg)
{
    s32 i;
    s32 *value;
    Video_SetRenderCallbacks(0, SpriteOam_QueueProjectedSprites, Sprite_RenderScaledList,
                             sub_08219624);
    WorldMap_InitializeBackground(menu);
    WorldMap_InitializeAnimations(menu);
    if (Script_SeekToKeyword(0x70)) {
        value = menu->entryValues;
        i = 8;
        do {
            *value++ = Script_GetValue();
        } while (--i >= 0);
    } else {
        s32 zero = 0;
        i = 8;
        do {
            menu->entryValues[i] = zero;
        } while (--i >= 0);
    }
    if (Script_SeekToKeyword(0x50))
        menu->initialSelection = Script_GetValue();
    else
        menu->initialSelection = 0;
    if (Script_SeekToKeyword(0x43))
        menu->cancelSelection = Script_GetValue();
    else
        menu->cancelSelection = -1;
    if (Script_SeekToKeyword(0x63) && Script_GetValue() == 0)
        menu->inputDisabled = 1;
    menu->availableEntries = 0;
    if (Script_SeekToKeyword(0x66)) {
        for (i = 0; i < 9; i++) {
            if (Script_GetValue())
                menu->availableEntries |= 1 << i;
        }
    }
    menu->selection = menu->initialSelection;
    menu->unknown524 = 0;
    if (Script_SeekToKeyword(0x62))
        WorldMap_ApplyPalettePhase(menu, Script_GetValue());
    else
        WorldMap_ApplyPalettePhase(menu, SunClock_GetPhase());
    WorldMap_CreateSelectionCursor(menu, menu->initialSelection);
    MenuCursor_Hide(menu->selectionCursor);
    WorldMap_BeginFadeIn(menu);
    return 0;
}

void WorldMap_StartTimedEffectFromScript(void)
{
    struct RegistrySelectionMenu *menu = (void *)gSelectionPanel;
    if (menu) {
        if (Script_SeekToKeyword(0x66)) {
            s32 mode = Script_GetValue();
            WorldMap_StartTimedEffect(menu, Script_GetKeywordValue(0x74, 7), mode);
        }
    }
}

void WorldMap_SetVisibleFromScript(void)
{
    struct RegistrySelectionMenu *menu = (void *)gSelectionPanel;
    struct MenuCursor **child;
    if (!menu)
        return;
    child = &menu->selectionCursor;
    if (!*child)
        return;
    if (Script_GetKeywordValue('v', 0)) {
        menu->inputDisabled = 0;
        MenuCursor_EnableInput(*child);
    } else {
        menu->inputDisabled = 1;
        MenuCursor_Hide(*child);
    }
}

struct RegistrySelectionMenu *WorldMap_Create(void *arg)
{
    struct RegistrySelectionMenu *menu;
    if (gSelectionPanel)
        return 0;
    menu = Task_Create(8, sizeof(*menu));
    if (menu) {
        Task_SetCallbacks(menu, (void (*)(void *))WorldMap_OnUpdate,
                          (void (*)(void *))WorldMap_Destroy);
        if (WorldMap_InitializeFromScript(menu, arg) < 0) {
            Task_Kill(menu);
            return 0;
        }
    }
    gSelectionPanel = (void *)menu;
    return menu;
}
