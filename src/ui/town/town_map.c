#include "ui/town/town_map.h"

void Palette_InterpolateBanks(void *, void *, void *, int, int);

void Bg_SetPriority(u32, s32);

void Bg_SetupLayers(int, int, void *, int, int, int, void *);

void Hud_SetMode(s32);

u32 SunClock_GetPhase(void);

void sub_082157EC(void);

u8 *sub_08216E54(u16);

void BgGfx_LoadTilemapAtPosition(s32, s32, s32);

void ScreenTransition_SendCommand(s32, s32, s32, s32, s32, s32, s32);

void Video_SetRenderCallbacks(int, void *, void *, void *);

void SpriteOam_QueueProjectedSprites(void);

void Sprite_RenderScaledList(void);

void sub_08219624(void);

void *memcpy(void *, const void *, u32);

extern const struct MenuCursorEntry gUnk_0824F878[11];

static inline u32 TestFlags(struct MenuCursor *cursor, u32 mask)
{
    return cursor->stateFlags & mask;
}

void TownMap_SetState(struct RegistryChoiceMenu *menu,
                           void (*update)(struct RegistryChoiceMenu *))
{
    menu->update = update;
    menu->timer = 0;
}

void TownMap_ApplyPaletteStep(struct RegistryChoiceMenu *transition, u32 step)
{
    u16 *palettes = transition->palettes;
    switch (step) {
        case 0:
            Palette_InterpolateBanks(gBgPaletteBuffer, palettes, palettes + 32, 48, 6);
            break;
        case 1:
            Palette_InterpolateBanks(gBgPaletteBuffer, palettes, palettes + 32, 16, 6);
            break;
        case 2:
            CpuSet(palettes, gBgPaletteBuffer, 0x04000008);
            break;
        case 3:
            Palette_InterpolateBanks(gBgPaletteBuffer, palettes, palettes + 16, 16, 6);
            break;
        case 4:
            Palette_InterpolateBanks(gBgPaletteBuffer, palettes + 16, palettes + 32, 48, 6);
            break;
        case 5:
            CpuSet(palettes + 32, gBgPaletteBuffer, 0x04000008);
            break;
    }
}

void TownMap_UpdateAnimations(struct RegistryChoiceMenu *menu)
{
    void *data = &menu->resource;
    struct AnimSprite *anim = &menu->animations[1];
    struct MenuCursorEntry *offset;
    if (menu->selection >= 0 && TestFlags(menu->cursor, 2)) {
        anim->flags &= ~1;
        Anim_LoadKeyframe(anim, data, (u16)(menu->selection + 0x2B), 0);
    } else {
        anim->flags |= 1;
    }
    anim = &menu->animations[3];
    offset = MenuCursor_GetEntry(menu->cursor, menu->initialSelection);
    if (offset) {
        anim->width = offset->x;
        anim->height = offset->y;
    } else {
        anim->width = 0;
        anim->height = 0;
    }
    anim = &menu->animations[4];
    if (menu->selection == 1)
        Anim_Step(anim, data);
}

void TownMap_ConfirmSelection(struct RegistryChoiceMenu *menu)
{
    if (gPadState[1] & 1) {
        if (TestFlags(menu->cursor, 2) && menu->selection >= 0) {
            if (menu->selection == 1 && menu->alternateScript) {
                Script_Call(menu->alternateScript, 0);
            } else if (menu->entryScripts[menu->selection]) {
                TownMap_StartClosingFade(menu);
                MenuCursor_SetSelectionFlag(menu->cursor);
            }
        }
    }
}

void TownMap_InitAnimations(struct RegistryChoiceMenu *menu)
{
    struct AnimSpriteResource *resource = ScriptProc_Start(0xcb05, 0xa2db);
    if (resource) {
        struct AnimSprite *anim;
        s32 count;
        menu->resource = *resource;
        ResourceHeader_Relocate(&menu->resource, resource);
        anim = menu->animations;
        for (count = 0; count < 5; count++) {
            Widget_InitLinked(anim, &menu->resource, 0, 0x10, 2, 0, 0x3c, 0);
            anim->attribute1A = 3;
            anim++;
        }
        Anim_LoadKeyframe(&menu->animations[2], &menu->resource, 0x26, 0);
        menu->animations[2].attribute1A = 1;
        Anim_LoadKeyframe(&menu->animations[0], &menu->resource, 0x27, 0);
        Anim_LoadKeyframe(&menu->animations[1], &menu->resource, 0x2b, 0);
        menu->animations[1].attribute1A = 2;
        Anim_LoadKeyframe(&menu->animations[3], &menu->resource, 0, 0);
        menu->animations[3].attribute1A = 2;
        Anim_SetKeyframes(&menu->animations[4], &menu->resource, 4, 1, 0);
        Anim_Step(&menu->animations[4], &menu->resource);
    }
}

void TownMap_InitBackground(struct RegistryChoiceMenu *menu)
{
    u8 *resource;
    s32 layers[2];
    {
        u16 *control = &gDispCntShadow;
        u32 mask = ~0xF00;
        u32 flags = *control;
        mask &= flags;
        *control = mask;
    }
    sub_082157EC();
    Hud_SetMode(7);
    {
        u32 flags = 0xC00;
        flags |= gDispCntShadow;
        flags |= 0x100;
        gDispCntShadow = flags;
    }
    menu->background = sub_08216E54(0xDC87);
    layers[0] = 1;
    layers[1] = 0;
    Bg_SetupLayers(1, 0, menu->background, 0, 0, 2, layers);
    Bg_SetPriority(3, 3);
    Bg_SetPriority(2, 2);
    resource = ScriptProc_Start(0x92B3, 0x771E);
    menu->palettes = (u16 *)(resource + 0x14);
    TownMap_ApplyPaletteStep(menu, SunClock_GetPhase());
    CpuSet(resource + 0x74, gBgPaletteBank3, 0x04000008);
}

void TownMap_ConfigureDisplay(void)
{
    BgGfx_LoadTilemapAtPosition(3, 8, 0x20);
    BgGfx_LoadTilemapAtPosition(2, 0, 0);
    BgGfx_LoadTilemapAtPosition(0, 0, 0);
    Hud_SetMode(7);
}

void TownMap_UpdateSelection(struct RegistryChoiceMenu *menu)
{
    s32 selection;
    if (menu->cursor == 0)
        selection = -1;
    else
        selection = menu->cursor->selectedIndex;
    menu->selection = selection;
    TownMap_ConfirmSelection(menu);
}

void TownMap_UpdateOpeningDelay(struct RegistryChoiceMenu *menu)
{
    s32 selection;
    if (menu->cursor == 0)
        selection = -1;
    else
        selection = menu->cursor->selectedIndex;
    menu->selection = selection;
    menu->timer++;
    if (menu->timer > 0x3c)
        TownMap_StartSelection(menu);
}

void TownMap_UpdateScriptDelay(struct RegistryChoiceMenu *menu)
{
    menu->timer++;
    if (menu->timer > 0x3c)
        Script_Call(menu->entryScripts[menu->selection], 0);
}

void TownMap_StartSelection(struct RegistryChoiceMenu *menu)
{
    TownMap_SetState(menu, TownMap_UpdateSelection);
}

void TownMap_StartOpeningFade(struct RegistryChoiceMenu *menu)
{
    ScreenTransition_SendCommand(2, 5, 4, 4, 4, 0xFFFF, 0);
    TownMap_SetState(menu, TownMap_UpdateOpeningDelay);
}

void TownMap_StartClosingFade(struct RegistryChoiceMenu *menu)
{
    ScreenTransition_SendCommand(3, 5, 4, 4, 4, 0xFFFF, 0);
    TownMap_SetState(menu, TownMap_UpdateScriptDelay);
}

void TownMap_OnUpdate(struct RegistryChoiceMenu *menu)
{
    TownMap_ConfigureDisplay();
    TownMap_UpdateAnimations(menu);
    if (menu->update)
        Callback_InvokeActor(menu, menu->update);
}

void TownMap_OnDestroy(struct RegistryChoiceMenu *menu)
{
    struct AnimSprite *animation = menu->animations;
    s32 i = 4;
    do {
        if (animation->active)
            Anim_Remove(animation);
        animation++;
        i--;
    } while (i >= 0);
    {
        struct MenuCursor **cursor = &menu->cursor;
        if (*cursor)
            *cursor = 0;
    }
}

void TownMap_CreateSelectionCursor(struct RegistryChoiceMenu *menu, s32 selection)
{
    struct MenuCursorEntry entries[11];
    memcpy(entries, gUnk_0824F878, 0xb0);
    menu->cursor = MenuCursor_Create(1, selection, 0xb, entries);
}

s32 TownMap_InitFromScript(struct RegistryChoiceMenu *menu, void *arg)
{
    s32 i;
    s32 *entry;
    Video_SetRenderCallbacks(0, SpriteOam_QueueProjectedSprites, Sprite_RenderScaledList,
                             sub_08219624);
    TownMap_InitBackground(menu);
    TownMap_InitAnimations(menu);
    if (Script_SeekToKeyword('p')) {
        entry = menu->entryScripts;
        i = 10;
        do {
            *entry++ = Script_GetValue();
        } while (--i >= 0);
    } else {
        for (i = 0; i <= 10; i++)
            menu->entryScripts[i] = 0;
    }
    if (Script_SeekToKeyword('P'))
        menu->initialSelection = Script_GetValue();
    else
        menu->initialSelection = 0;
    if (Script_SeekToKeyword('t'))
        menu->alternateScript = Script_GetValue();
    else
        menu->alternateScript = 0;
    menu->selection = menu->initialSelection;
    TownMap_CreateSelectionCursor(menu, menu->initialSelection);
    TownMap_StartOpeningFade(menu);
    return 0;
}

struct RegistryChoiceMenu *TownMap_Create(void *arg)
{
    struct RegistryChoiceMenu *menu = Task_Create(8, sizeof(*menu));
    if (menu) {
        Task_SetCallbacks(menu, (void (*)(void *))TownMap_OnUpdate,
                          (void (*)(void *))TownMap_OnDestroy);
        if (TownMap_InitFromScript(menu, arg) < 0) {
            Task_Kill(menu);
            return 0;
        }
    }
    return menu;
}
