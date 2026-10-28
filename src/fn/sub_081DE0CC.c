// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "game_state.h"
#include "ram.h"
#include "render.h"

/* NameEntry_Initialize: reads the script's "b" (name slot) and "d" (initial name) keywords,
 * loads the slot's current name, counts its blank trailing pairs to place the cursor, then
 * sets up the keyboard backgrounds and six widgets. (guess) */

s32 Script_SeekToKeyword(s32);
s32 Script_GetValue(void);
u8 *Script_GetPc(void);
u32 Script_ReadString(u8 *);
s32 Script_ParseStringRef(const void *);
u8 *Text_LookupString(s32);
u8 *PlayerName_GetBuffer(void);
void TextEntry_CopyInitialName(u8 *, u8 *);
void TextEntry_DrawName(u8 *);
void NameEntry_UpdateCursor(u8 *);
void Bg_ClearTilemap(u32);
void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);
void *ScriptProc_Start(u32, u32);
void Bg_SetupLayers(u32, u32, void *, u32, u32, u32, u32 *);
void ResourceHeader_Relocate(u8 *, void *);
void Widget_InitLinked(u8 *, u8 *, u32, u32, u32, u32, u32, u32);
void Menu_StartFadeIn(u32);

s32 NameEntry_Initialize(u8 *s)
{
    s32 count;
    u32 *slot;
    u8 *name;
    u8 *str;
    s32 i;
    u32 value;
    void *res;
    register u8 *obj asm("r4");

    count = 0;
    *(u32 *)(s + 0x348) = count;
    slot = (u32 *)(s + 0x370);
    *slot = count;
    if (Script_SeekToKeyword(0x62))
        *slot = Script_GetValue();
    if (Script_SeekToKeyword(0x64)) {
        *(u32 *)(s + 0x394) = Script_ReadString(Script_GetPc());
        Script_GetPc();
    }
    if (gNamePlaceholderScript[4] == 1) {
        if (gNameEntrySavedState[*slot + 2] == 0)
            *(u32 *)(s + 0x34C) = 0x56;
        goto player;
    } else if (gNameEntrySavedState[*slot + 2] == 0) {
        /* The slot value stays in r2 here; a three-argument call reproduces that. */
        ((void (*)(u8 *, u8 *, u32))TextEntry_CopyInitialName)(s, s + 0x374, *slot);
    } else if (*slot == 0) {
    player:
        CpuSet(PlayerName_GetBuffer(), s + 0x374, 8);
    } else {
        CpuSet((u8 *)gGameState + 0x790, s + 0x374, 8);
    }
    name = s + 0x374;
    str = Text_LookupString(Script_ParseStringRef((const void *)gNamePlaceholderScript[0]) + 0x54);
    for (i = 8; i >= 0; i -= 2) {
        u8 *c = name + i;
        asm("" : "+r"(c));
        if ((c[0] == 0 && c[1] == 0) || (c[0] == str[0] && c[1] == str[1])) {
            count++;
            continue;
        }
        {
            s32 *pos = (s32 *)(s + 0x350);
            *pos = i / 2 + 1;
            if (*pos > 4)
                *pos = 4;
            break;
        }
    }
    if (count == 5) {
        TextEntry_CopyInitialName(s, s + 0x374);
        *(s32 *)(s + 0x350) = 4;
    }
    {
        u8 *table = gNameEntrySavedState;
        u32 *p = (u32 *)(s + 0x370);
        u32 idx = *p + 2;
        if (table[idx] == 0) {
            if (gNamePlaceholderScript[4])
                *(u32 *)(s + 0x34C) = 0x56;
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
            u16 *h = (u16 *)(s + 0x358);
            *h = zero1 = 0;
        }
        *(u16 *)(s + 0x35A) = zero1;
        {
            u32 *a = (u32 *)(s + 0x35C);
            u32 *b = (u32 *)(s + 0x360);
            *b = one1 = 1;
            *a = one1;
        }
        *(u16 *)(s + 0x364) = zero1;
        Bg_ClearTilemap(0);
        BgGfx_LoadTilemapSimple(0, 0, 0, 0, zero1);
        Bg_ClearTilemap(2);
        BgGfx_LoadTilemapSimple(2, 0, 0, 0, zero1);
        Bg_ClearTilemap(3);
        BgGfx_LoadTilemapSimple(3, 0, 0, 0, zero1);
        bg = ScriptProc_Start(0xC091, 0x3536);
        *(void **)(s + 0x354) = bg;
        value = zero1;
        Bg_SetupLayers(2, 0, bg, 0, zero1, one1, &value);
        BgGfx_LoadTilemapSimple(3, 0, 0, 0, zero1);
        CpuSet((u8 *)ScriptProc_Start(0x92B3, 0x204) + 0x14, gBgPaletteBuffer, 0x100);
        res = ScriptProc_Start(0xCB05, 0xE2AB);
        ResourceHeader_Relocate(s + 0x78, res);
        if (*(u32 *)(s + 0x370) == 0)
            Widget_InitLinked(s + 0x18, s + 0x78, 0x33, 0x10, zero1, zero1, zero1, zero1);
        else
            Widget_InitLinked(s + 0x18, s + 0x78, 0x3D, 0x10, zero1, zero1, zero1, zero1);
    }
    {
        u32 zero2;
        u32 one2;
        obj = s + 0x100;
        ResourceHeader_Relocate(obj, res);
        zero2 = 0;
        Widget_InitLinked(s + 0xA0, obj, 0x16, 0x10, zero2, zero2, zero2, zero2);
        obj = s + 0x210;
        ResourceHeader_Relocate(obj, res);
        Widget_InitLinked(s + 0x1B0, obj, 0x18, 0x10, zero2, zero2, zero2, zero2);
        obj = s + 0x188;
        ResourceHeader_Relocate(obj, res);
        Widget_InitLinked(s + 0x128, obj, 0x17, 0x10, zero2, zero2, zero2, zero2);
        {
            u32 *f = (u32 *)(s + 0x130);
            u32 v = *f;
            one2 = 1;
            *f = v | one2;
        }

        {
            u32 *save = (u32 *)(s + 0x39C);
            u32 *ptr = (u32 *)(s + 0xE8);
            u32 old = *ptr;
            *save = old;
            obj = s + 0x3A0;
            CpuSet((void *)old, obj, 0x10);
            *ptr = (u32)obj;
        }
        *(u8 **)(s + 0x1F8) = obj;
        *(u8 **)(s + 0x170) = obj;
        {
            u32 bits = 0xE000;
            *(u16 *)(s + 0xDA) = bits;
            *(u16 *)(s + 0x1EA) = bits + 2;
        }
        {
            u16 *halfB = (u16 *)(s + 0x162);
            u32 bitsB = 0xE001;
            *halfB = bitsB;
        }
        res = ScriptProc_Start(0xCB05, 0x530D);
        obj = s + 0x298;
        ResourceHeader_Relocate(obj, res);
        Widget_InitLinked(s + 0x238, obj, 0x8C, 0x10, one2, zero2, zero2, zero2);
        obj = s + 0x320;
        ResourceHeader_Relocate(obj, res);
        Widget_InitLinked(s + 0x2C0, obj, 0x8D, 0x10, zero2, zero2, zero2, zero2);
        *(u32 *)(s + 0x240) &= ~1;
        *(u32 *)(s + 0x2C8) |= one2;
        {
            u32 *save = (u32 *)(s + 0x398);
            u32 *ptr = (u32 *)(s + 0x308);
            u32 old = *ptr;
            *save = old;
            obj = s + 0x3C0;
            CpuSet((void *)old, obj, 0x10);
            *ptr = (u32)obj;
        }
        {
            u16 *halfB = (u16 *)(s + 0x2FA);
            u16 bitsB = 0xE010;
            *halfB = bitsB;
        }
    }
    TextEntry_DrawName(s + 0x374);
    NameEntry_UpdateCursor(s);
    Menu_StartFadeIn(*(u32 *)(s + 0x36C) = gNamePlaceholderScript[5]);
    return 0;
}
