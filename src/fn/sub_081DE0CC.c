#include "name_entry.h"
#include "game_state.h"
#include "ram.h"
#include "render.h"

/* NameEntry_Initialize: reads the script's "b" (name slot) and "d" (initial name) keywords,
 * loads the slot's current name, counts its blank trailing pairs to place the cursor, then
 * sets up the keyboard backgrounds and six widgets. (guess) */

s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
u8 *Script_GetPc(void);
const u8 *Script_ReadString(const u8 *);
s32 Script_ParseStringRef(const void *);
u8 *Text_LookupString(s32);
u8 *PlayerName_GetBuffer(void);
void TextEntry_DrawName(u8 *);
void NameEntry_UpdateCursor(u8 *);
void Bg_ClearTilemap(u32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void *ScriptProc_Start(u32, u32);
void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);
s32 ResourceHeader_Relocate(void *, void *);
s32 Widget_InitLinked(void *, void *, u16, u32, u8, u8, u8, const void *);
void Menu_StartFadeIn(u32);

s32 NameEntry_Initialize(struct NameEntryTask *menu)
{
    struct NameEntryTask *screen = menu;
    u8 *nameResource;
    struct TextEntryWidget *nameWidget;
    u16 **keyPalettePtr;
    u16 *keyPaletteBank;
    struct TextEntryWidget *keyWidget;
    s32 count;
    u32 *slot;
    u8 *name;
    u8 *str;
    s32 i;
    u32 value;
    void *res;
    u8 *keyGraphics;
    u16 *keyColors;
    u8 *cursorGraphics;
    u16 *cursorColors;

    count = 0;
    menu->unk348 = count;
    slot = &screen->slot;
    *slot = count;
    if (Script_SeekToKeyword(0x62))
        *slot = Script_GetValue();
    if (Script_SeekToKeyword(0x64)) {
        menu->initialName = Script_ReadString(Script_GetPc());
        Script_GetPc();
    }
    if (gNamePlaceholderScript[4] == 1) {
        if (gNameEntrySavedState[*slot + 2] == 0) {
            screen->selection = 0x56;
        }
        CpuSet(PlayerName_GetBuffer(), screen->name, 8);
    } else if (gNameEntrySavedState[*slot + 2] == 0) {
        TextEntry_CopyInitialName(screen, screen->name);
    } else if (*slot == 0) {
        CpuSet(PlayerName_GetBuffer(), screen->name, 8);
    } else {
        CpuSet(gGameState->playerName + 16, screen->name, 8);
    }
    name = screen->name;
    str = Text_LookupString(Script_ParseStringRef((const void *)gNamePlaceholderScript[0]) + 0x54);
    i = 8;
    nameResource = screen->widgets[0].resource;
    nameWidget = screen->widgets;
    keyWidget = &screen->widgets[1];
    keyPalettePtr = &screen->widgets[1].palette;
    keyPaletteBank = &screen->widgets[1].paletteBank;
    for (; i >= 0; i -= 2) {
        u8 *c = name + i;
        if ((c[0] == 0 && c[1] == 0) || (c[0] == str[0] && c[1] == str[1])) {
            count++;
            continue;
        }
        {
            s32 *pos = &screen->position;
            *pos = i / 2 + 1;
            if (*pos > 4)
                *pos = 4;
            break;
        }
    }
    if (count == 5) {
        TextEntry_CopyInitialName(screen, screen->name);
        screen->position = 4;
    }
    {
        u8 *table = gNameEntrySavedState;
        u32 *p = &screen->slot;
        u32 idx = *p + 2;
        if (table[idx] == 0) {
            if (gNamePlaceholderScript[4])
                screen->selection = 0x56;
            {
                u8 *a = table + (*p + 2);
                *a = 1;
            }
        }
    }
    {
        u32 zero1;
        u32 one1;
        void *bg;
        {
            u16 *h = &screen->verticalHistory;
            *h = zero1 = 0;
        }
        screen->horizontalHistory = zero1;
        {
            s32 *a = &screen->verticalRepeat;
            s32 *b = &screen->horizontalRepeat;
            *b = one1 = 1;
            *a = one1;
        }
        screen->state = zero1;
        Bg_ClearTilemap(0);
        BgGfx_LoadTilemapSimple(0, 0, 0, 0, zero1);
        Bg_ClearTilemap(2);
        BgGfx_LoadTilemapSimple(2, 0, 0, 0, zero1);
        Bg_ClearTilemap(3);
        BgGfx_LoadTilemapSimple(3, 0, 0, 0, zero1);
        bg = ScriptProc_Start(0xC091, 0x3536);
        screen->bg = bg;
        value = zero1;
        Bg_SetupLayers(2, 0, bg, 0, zero1, one1, &value);
        BgGfx_LoadTilemapSimple(3, 0, 0, 0, zero1);
        CpuSet((u8 *)ScriptProc_Start(0x92B3, 0x204) + 0x14, gBgPaletteBuffer, 0x100);
        res = ScriptProc_Start(0xCB05, 0xE2AB);
        ResourceHeader_Relocate(nameResource, res);
        if (screen->slot == 0)
            Widget_InitLinked(nameWidget, nameResource, 0x33, 0x10, zero1, zero1, zero1, NULL);
        else
            Widget_InitLinked(nameWidget, nameResource, 0x3D, 0x10, zero1, zero1, zero1, NULL);
    }
    {
        u32 zero2;
        u32 one2;
        keyGraphics = screen->widgets[1].resource;
        ResourceHeader_Relocate(keyGraphics, res);
        zero2 = 0;
        Widget_InitLinked(keyWidget, keyGraphics, 0x16, 0x10, zero2, zero2, zero2, NULL);
        keyGraphics = screen->widgets[3].resource;
        ResourceHeader_Relocate(keyGraphics, res);
        Widget_InitLinked(&screen->widgets[3], keyGraphics, 0x18, 0x10, zero2, zero2, zero2, NULL);
        keyGraphics = screen->widgets[2].resource;
        ResourceHeader_Relocate(keyGraphics, res);
        Widget_InitLinked(&screen->widgets[2], keyGraphics, 0x17, 0x10, zero2, zero2, zero2, NULL);
        {
            u32 *f = &screen->widgets[2].flags;
            u32 v = *f;
            one2 = 1;
            *f = v | one2;
        }

        {
            u16 **save = &screen->savedKeyPalette;
            u16 **ptr = keyPalettePtr;
            u16 *old = *ptr;
            *save = old;
            keyColors = screen->keyColors;
            CpuSet(old, keyColors, 0x10);
            *ptr = keyColors;
        }
        screen->widgets[3].palette = keyColors;
        screen->widgets[2].palette = keyColors;
        {
            u32 bits = 0xE000;
            *keyPaletteBank = bits;
            {
                u16 *bank = &screen->widgets[3].paletteBank;
                *bank = bits + 2;
            }
        }
        {
            u16 *halfB = &screen->widgets[2].paletteBank;
            u32 bitsB = 0xE001;
            *halfB = bitsB;
        }
        res = ScriptProc_Start(0xCB05, 0x530D);
        cursorGraphics = screen->widgets[4].resource;
        ResourceHeader_Relocate(cursorGraphics, res);
        Widget_InitLinked(&screen->widgets[4], cursorGraphics, 0x8C, 0x10, one2, zero2, zero2,
                          NULL);
        cursorGraphics = screen->widgets[5].resource;
        ResourceHeader_Relocate(cursorGraphics, res);
        Widget_InitLinked(&screen->widgets[5], cursorGraphics, 0x8D, 0x10, zero2, zero2, zero2,
                          NULL);
        screen->widgets[4].flags &= ~1;
        screen->widgets[5].flags |= one2;
        {
            u16 **save = &screen->savedCursorPalette;
            u16 **ptr = &screen->widgets[5].palette;
            u16 *old = *ptr;
            *save = old;
            cursorColors = screen->cursorColors;
            CpuSet(old, cursorColors, 0x10);
            *ptr = cursorColors;
        }
        {
            u16 *halfB = &screen->widgets[5].paletteBank;
            u16 bitsB = 0xE010;
            *halfB = bitsB;
        }
    }
    TextEntry_DrawName(screen->name);
    NameEntry_UpdateCursor((u8 *)screen);
    Menu_StartFadeIn(screen->fadeShift = gNamePlaceholderScript[5]);
    return 0;
}
