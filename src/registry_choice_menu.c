#include "registry_choice_menu.h"

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

void Registry3F74_SetState(struct RegistryChoiceMenu *menu,
                           void (*update)(struct RegistryChoiceMenu *))
{
    menu->update = update;
    menu->timer = 0;
}

void Registry3F74_ApplyPaletteStep(struct RegistryChoiceMenu *transition, u32 step)
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

void Registry3F74_UpdateAnimations(struct RegistryChoiceMenu *menu)
{
    void *data = &menu->resource;
    struct SceneInputWidget *anim = &menu->animations[1];
    struct MenuCursorEntry *offset;
    if (menu->selection >= 0 && TestFlags(menu->cursor, 2)) {
        anim->flags &= ~1;
        Anim_LoadKeyframe(anim, data, (u16)(menu->selection + 0x2B), 0);
    } else {
        anim->flags |= 1;
    }
    anim = &menu->animations[3];
    offset = Registry3F74_FindFrameOffset(menu->cursor, menu->initialSelection);
    if (offset) {
        anim->screenOffsetX = offset->x;
        anim->screenOffsetY = offset->y;
    } else {
        anim->screenOffsetX = 0;
        anim->screenOffsetY = 0;
    }
    anim = &menu->animations[4];
    if (menu->selection == 1)
        Anim_Step(anim, data);
}

void Registry3F74_ConfirmSelection(struct RegistryChoiceMenu *menu)
{
    if (gPadState[1] & 1) {
        if (TestFlags(menu->cursor, 2) && menu->selection >= 0) {
            if (menu->selection == 1 && menu->alternateScript) {
                Script_Call(menu->alternateScript, 0);
            } else if (menu->entryScripts[menu->selection]) {
                Registry3F74_StartClosingFade(menu);
                MenuCursor_SetSelectionFlag(menu->cursor);
            }
        }
    }
}

void Registry3F74_InitAnimations(struct RegistryChoiceMenu *menu)
{
    struct SceneInputResource *resource = ScriptProc_Start(0xcb05, 0xa2db);
    if (resource) {
        struct SceneInputWidget *anim;
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

void Registry3F74_InitBackground(struct RegistryChoiceMenu *menu)
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
    Registry3F74_ApplyPaletteStep(menu, SunClock_GetPhase());
    CpuSet(resource + 0x74, gBgPaletteBank3, 0x04000008);
}

void Registry3F74_ConfigureDisplay(void)
{
    BgGfx_LoadTilemapAtPosition(3, 8, 0x20);
    BgGfx_LoadTilemapAtPosition(2, 0, 0);
    BgGfx_LoadTilemapAtPosition(0, 0, 0);
    Hud_SetMode(7);
}

void Registry3F74_UpdateSelection(struct RegistryChoiceMenu *menu)
{
    s32 selection;
    if (menu->cursor == 0)
        selection = -1;
    else
        selection = menu->cursor->selectedIndex;
    menu->selection = selection;
    Registry3F74_ConfirmSelection(menu);
}

void Registry3F74_UpdateOpeningDelay(struct RegistryChoiceMenu *menu)
{
    s32 selection;
    if (menu->cursor == 0)
        selection = -1;
    else
        selection = menu->cursor->selectedIndex;
    menu->selection = selection;
    menu->timer++;
    if (menu->timer > 0x3c)
        Registry3F74_StartSelection(menu);
}

void Registry3F74_UpdateScriptDelay(struct RegistryChoiceMenu *menu)
{
    menu->timer++;
    if (menu->timer > 0x3c)
        Script_Call(menu->entryScripts[menu->selection], 0);
}

void Registry3F74_StartSelection(struct RegistryChoiceMenu *menu)
{
    Registry3F74_SetState(menu, Registry3F74_UpdateSelection);
}

void Registry3F74_StartOpeningFade(struct RegistryChoiceMenu *menu)
{
    ScreenTransition_SendCommand(2, 5, 4, 4, 4, 0xFFFF, 0);
    Registry3F74_SetState(menu, Registry3F74_UpdateOpeningDelay);
}

void Registry3F74_StartClosingFade(struct RegistryChoiceMenu *menu)
{
    ScreenTransition_SendCommand(3, 5, 4, 4, 4, 0xFFFF, 0);
    Registry3F74_SetState(menu, Registry3F74_UpdateScriptDelay);
}

void Registry3F74_OnUpdate(struct RegistryChoiceMenu *menu)
{
    Registry3F74_ConfigureDisplay();
    Registry3F74_UpdateAnimations(menu);
    if (menu->update)
        Callback_InvokeActor(menu, menu->update);
}

void Registry3F74_OnDestroy(struct RegistryChoiceMenu *menu)
{
    struct SceneInputWidget *animation = menu->animations;
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

void Registry3F74_CreateSelectionCursor(struct RegistryChoiceMenu *menu, s32 selection)
{
    struct MenuCursorEntry entries[11];
    memcpy(entries, gUnk_0824F878, 0xb0);
    menu->cursor = MenuCursor_Create(1, selection, 0xb, entries);
}

s32 Registry3F74_InitFromScript(struct RegistryChoiceMenu *menu, void *arg)
{
    s32 i;
    s32 *entry;
    Video_SetRenderCallbacks(0, SpriteOam_QueueProjectedSprites, Sprite_RenderScaledList,
                             sub_08219624);
    Registry3F74_InitBackground(menu);
    Registry3F74_InitAnimations(menu);
    if (Script_SeekToKeyword('p')) {
        entry = menu->entryScripts;
        i = 10;
        do {
            *entry++ = Script_GetValue();
        } while (--i >= 0);
    } else {
        s32 zero = 0;
        for (i = 10; i >= 0; i--)
            menu->entryScripts[i] = zero;
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
    Registry3F74_CreateSelectionCursor(menu, menu->initialSelection);
    Registry3F74_StartOpeningFade(menu);
    return 0;
}

struct RegistryChoiceMenu *Registry3F74_Create(void *arg)
{
    struct RegistryChoiceMenu *menu = Task_Create(8, sizeof(*menu));
    if (menu) {
        Task_SetCallbacks(menu, (void (*)(void *))Registry3F74_OnUpdate,
                          (void (*)(void *))Registry3F74_OnDestroy);
        if (Registry3F74_InitFromScript(menu, arg) < 0) {
            Task_Kill(menu);
            return 0;
        }
    }
    return menu;
}
