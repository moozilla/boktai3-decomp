#include "name_entry.h"
#include "game_state.h"
#include "global.h"
#include "engine.h"
#include "ram.h"
#include "input.h"
#include "render.h"

u8 *PlayerName_GetBuffer(void);

void TextEntry_CopyInitialName(struct NameEntryTask *p, u8 *out)
{
    const u8 *src;
    s32 i;
    if (p->initialName)
        src = p->initialName;
    else if (!p->slot)
        src = PlayerName_GetBuffer();
    else
        src = gGameState->playerName + 16;
    i = 0;
    do {
        u8 c = src[i];
        if (!c)
            break;
        out[i] = c;
        i++;
    } while (i <= 10);
    out[i] = 0;
}

void NameEntry_CopyNameBuffer(const void *src, void *dest)
{
    CpuSet(src, dest, 8);
}

void Menu_DrawText(u32, u32, const u8 *);

void Menu_EraseRect(u32, u32, u32, u32);

void TextEntry_DrawName(const void *src)
{
    u8 text[32];
    u8 *p;
    s32 i;
    CpuSet(src, text, 0x04000008);
    p = text;
    i = 4;
    do {
        if (!p[0] && !p[1]) {
            u8 *placeholder = Text_LookupString(
                Script_ParseStringRef((*(const void **)gNamePlaceholderScript)) + 0x54);
            p[0] = placeholder[0];
            p[1] = placeholder[1];
        }
        p += 2;
    } while (--i >= 0);
    Menu_EraseRect(18, 1, 10, 2);
    Menu_DrawText(18, 1, text);
}

/* NameEntry_UpdateCursor: positions the keyboard cursor sprite for the selected key (80 character cells in two 5x8 blocks, then the page/case/back/end buttons) */

extern const u8 gUnk_0824FCBC[];

extern const u8 gUnk_0824FCC4[];

s32 Mod(s32, s32);

s32 Div(s32, s32);

void NameEntry_UpdateCursor(struct NameEntryTask *p)
{
    u8 columns[8];
    u8 rows[8];
    s32 col, row;

    memcpy(columns, gUnk_0824FCBC, 8);
    memcpy(rows, gUnk_0824FCC4, 8);
    p->widgets[1].flags &= ~1;
    p->widgets[2].flags |= 1;
    p->widgets[5].flags |= 1;
    if (p->selection < 80) {
        col = Mod(p->selection, 5);
        if (p->selection < 40) {
            row = Div(p->selection, 5);
            p->widgets[1].x = columns[col];
            p->widgets[1].y = rows[row];
        } else {
            row = Div(p->selection - 40, 5);
            p->widgets[1].x = columns[col] + 0x58;
            p->widgets[1].y = rows[row];
        }
    } else {
        switch (p->selection - 80) {
            case 0:
                p->widgets[1].x = 0xB0;
                p->widgets[1].y = 0;
                break;
            case 1:
                p->widgets[1].x = 0xC0;
                p->widgets[1].y = 0;
                break;
            case 2:
                p->widgets[1].x = 0xB0;
                p->widgets[1].y = 0x10;
                break;
            case 3:
                p->widgets[1].x = 0xC0;
                p->widgets[1].y = 0x10;
                break;
            case 4:
                p->widgets[2].x = 0;
                p->widgets[2].y = -16;
                p->widgets[2].flags &= ~1;
                p->widgets[1].flags |= 1;
                break;
            case 5:
                p->widgets[2].x = 0;
                p->widgets[2].y = 0;
                p->widgets[1].flags |= 1;
                p->widgets[2].flags &= ~1;
                break;
            case 6:
                p->widgets[5].x = 0;
                p->widgets[5].y = 0;
                p->widgets[1].flags |= 1;
                p->widgets[5].flags &= ~1;
                break;
        }
    }
}

void NameEntry_UpdateCursor(struct NameEntryTask *);

void NameEntry_MoveCursorUp(struct NameEntryTask *p)
{
    switch (p->selection) {
        case 0:
            p->selection = 0x23;
            break;
        case 1:
            p->selection = 0x24;
            break;
        case 2:
            p->selection = 0x25;
            break;
        case 3:
            p->selection = 0x26;
            break;
        case 4:
            p->selection = 0x27;
            break;
        case 40:
            p->selection = 0x4b;
            break;
        case 41:
            p->selection = 0x4c;
            break;
        case 42:
            p->selection = 0x4d;
            break;
        case 43:
            p->selection = 0x4e;
            break;
        case 44:
            p->selection = 0x4f;
            break;
        case 80:
            p->selection = 0x56;
            break;
        case 81:
            p->selection = 0x56;
            break;
        case 82:
            p->selection = 0x50;
            break;
        case 83:
            p->selection = 0x51;
            break;
        case 84:
            p->selection = 0x52;
            break;
        case 85:
            p->selection = 0x54;
            break;
        case 86:
            p->selection = 0x55;
            break;
        default:
            p->selection -= 5;
            break;
    }
    NameEntry_UpdateCursor(p);
    Sound_PlaySong(0xdc);
}

void NameEntry_MoveCursorDown(struct NameEntryTask *p)
{
    switch (p->selection) {
        case 0x23:
            p->selection = 0x0;
            break;
        case 0x24:
            p->selection = 0x1;
            break;
        case 0x25:
            p->selection = 0x2;
            break;
        case 0x26:
            p->selection = 0x3;
            break;
        case 0x27:
            p->selection = 0x4;
            break;
        case 0x4b:
            p->selection = 0x28;
            break;
        case 0x4c:
            p->selection = 0x29;
            break;
        case 0x4d:
            p->selection = 0x2a;
            break;
        case 0x4e:
            p->selection = 0x2b;
            break;
        case 0x4f:
            p->selection = 0x2c;
            break;
        case 0x50:
            p->selection = 0x52;
            break;
        case 0x51:
            p->selection = 0x53;
            break;
        case 0x52:
            p->selection = 0x54;
            break;
        case 0x53:
            p->selection = 0x54;
            break;
        case 0x54:
            p->selection = 0x55;
            break;
        case 0x55:
            p->selection = 0x56;
            break;
        case 0x56:
            p->selection = 0x50;
            break;
        default:
            if (p->characterPage == 1 && p->selection == 0x4a)
                p->selection = 0x4e;
            else
                p->selection += 5;
            break;
    }
    NameEntry_UpdateCursor(p);
    Sound_PlaySong(0xdc);
}

void NameEntry_MoveCursorRight(struct NameEntryTask *p)
{
    switch (p->selection) {
        case 0x4:
            p->selection = 0x28;
            break;
        case 0x9:
            p->selection = 0x2d;
            break;
        case 0xe:
            p->selection = 0x32;
            break;
        case 0x13:
            p->selection = 0x37;
            break;
        case 0x18:
            p->selection = 0x3c;
            break;
        case 0x1d:
            p->selection = 0x41;
            break;
        case 0x22:
            p->selection = 0x46;
            break;
        case 0x27:
            p->selection = 0x4b;
            break;
        case 0x2c:
            p->selection = 0x50;
            break;
        case 0x31:
            p->selection = 0x52;
            break;
        case 0x36:
            p->selection = 0x54;
            break;
        case 0x3b:
            p->selection = 0x55;
            break;
        case 0x40:
            p->selection = 0x56;
            break;
        case 0x45:
            p->selection = 0x56;
            break;
        case 0x4a:
            p->selection = 0x56;
            break;
        case 0x4f:
            p->selection = 0x56;
            break;
        case 0x51:
            p->selection = 0x0;
            break;
        case 0x53:
            p->selection = 0x5;
            break;
        case 0x54:
            p->selection = 0xa;
            break;
        case 0x55:
            p->selection = 0xf;
            break;
        case 0x56:
            p->selection = 0x19;
            break;
        default:
            if (p->characterPage == 1 && p->selection == 0x4e)
                p->selection = 0x4a;
            else
                p->selection += 1;
            break;
    }
    NameEntry_UpdateCursor(p);
    Sound_PlaySong(0xdc);
}

void NameEntry_MoveCursorLeft(struct NameEntryTask *p)
{
    switch (p->selection) {
        case 0x0:
            p->selection = 0x51;
            break;
        case 0x5:
            p->selection = 0x53;
            break;
        case 0xa:
            p->selection = 0x54;
            break;
        case 0xf:
            p->selection = 0x55;
            break;
        case 0x14:
            p->selection = 0x56;
            break;
        case 0x19:
            p->selection = 0x56;
            break;
        case 0x1e:
            p->selection = 0x56;
            break;
        case 0x23:
            p->selection = 0x56;
            break;
        case 0x28:
            p->selection = 0x4;
            break;
        case 0x2d:
            p->selection = 0x9;
            break;
        case 0x32:
            p->selection = 0xe;
            break;
        case 0x37:
            p->selection = 0x13;
            break;
        case 0x3c:
            p->selection = 0x18;
            break;
        case 0x41:
            p->selection = 0x1d;
            break;
        case 0x46:
            p->selection = 0x22;
            break;
        case 0x4b:
            p->selection = 0x27;
            break;
        case 0x50:
            p->selection = 0x2c;
            break;
        case 0x52:
            p->selection = 0x31;
            break;
        case 0x54:
            p->selection = 0x36;
            break;
        case 0x55:
            p->selection = 0x3b;
            break;
        case 0x56:
            p->selection = 0x45;
            break;
        default:
            p->selection -= 1;
            break;
    }
    NameEntry_UpdateCursor(p);
    Sound_PlaySong(0xdc);
}

int Script_ParseStringRef(const void *);

u8 *Text_LookupString(int);

void TextEntry_DrawName(const void *);

u32 NameEntry_AppendChar(struct NameEntryTask *p)
{
    u8 *dst = &p->name[p->position * 2];
    u8 *src;
    u32 r;
    const void *ref;
    if (p->characterPage == 0)
        ref = gNamePlaceholderScript[0];
    else
        ref = gNamePlaceholderScript[1];
    src = Text_LookupString(Script_ParseStringRef(ref) + p->selection);
    dst[0] = src[0];
    dst[1] = src[1];
    if (p->position <= 3) {
        p->position++;
        r = 0;
    } else
        r = 1;
    TextEntry_DrawName(p->name);
    return r;
}

void Menu_SetBackgroundMap(u32 p, u32 b);

void NameEntry_UpdateCursor(struct NameEntryTask *s);

void NameEntry_ToggleCharacterPage(struct NameEntryTask *s)
{
    if (s->characterPage == 0) {
        if (s->selection == 0x4f) {
            s->selection = 0x4e;
        }
        s->characterPage = 1;
        Menu_SetBackgroundMap(s->bg, 1);
    } else {
        s->characterPage = 0;
        Menu_SetBackgroundMap(s->bg, 0);
    }
    NameEntry_UpdateCursor(s);
}

/* NameEntry_Update: per-frame task callback. State 0 handles the d-pad (with key repeat),
 * B (erase, or cancel on an empty name), SELECT (page toggle), START (jump to End) and A
 * (type / toggle / confirm); confirming stores the name and fades out. State 1 waits for
 * the fade, hides the widgets, runs the follow-up script and kills the task. (guess) */

void TextEntry_DrawName(const void *);

s32 Script_ParseStringRef(const void *);

u8 *Text_LookupString(s32);

s32 NameEntry_EraseCharacter(struct NameEntryTask *p)
{
    u8 *ch = &p->name[p->position * 2];
    u8 *placeholder =
        Text_LookupString(Script_ParseStringRef((const void *)gNamePlaceholderScript[0]) + 0x54);
    s32 result = 0;
    if ((ch[0] == 0 && ch[1] == 0) || (ch[0] == placeholder[0] && ch[1] == placeholder[1])) {
        if (p->position == 0) {
            result = 1;
            goto draw;
        }
        p->position--;
        ch = &p->name[p->position * 2];
    }
    ch[0] = 0;
    ch[1] = 0;
draw:
    TextEntry_DrawName(p->name);
    return result;
}

/* NameEntry_Update: per-frame task callback. State 0 handles the d-pad (with key repeat),
 * B (erase, or cancel on an empty name), SELECT (page toggle), START (jump to End) and A
 * (type / toggle / confirm); confirming stores the name and fades out. State 1 waits for
 * the fade, hides the widgets, runs the follow-up script and kills the task. (guess) */

void NameEntry_MoveCursorUp(struct NameEntryTask *);

void NameEntry_MoveCursorDown(struct NameEntryTask *);

void NameEntry_MoveCursorRight(struct NameEntryTask *);

void NameEntry_MoveCursorLeft(struct NameEntryTask *);

s32 NameEntry_EraseCharacter(struct NameEntryTask *);

void NameEntry_ToggleCharacterPage(struct NameEntryTask *);

u32 NameEntry_AppendChar(struct NameEntryTask *);

void Menu_StartFadeOut(s32);

void Palette_UpdateHighlightPulse(void *, void *);

void NameEntry_Update(struct NameEntryTask *p)
{
    s32 r;
    switch (p->state) {
        case 0:
            p->verticalHistory <<= 1;
            p->horizontalHistory <<= 1;
            if (gPadState[0] & 0xC0)
                p->verticalHistory |= 1;
            if (gPadState[0] & 0x30)
                p->horizontalHistory |= 1;
            if (p->verticalHistory == 0xFFFF)
                p->verticalRepeat++;
            else
                p->verticalRepeat = 1;
            if (p->horizontalHistory == 0xFFFF)
                p->horizontalRepeat++;
            else
                p->horizontalRepeat = 1;
            if ((gPadState[1] & 0x40) || (Mod(p->verticalRepeat, 5) == 0 && (gPadState[0] & 0x40)))
                NameEntry_MoveCursorUp(p);
            if ((gPadState[1] & 0x80) || (Mod(p->verticalRepeat, 5) == 0 && (gPadState[0] & 0x80)))
                NameEntry_MoveCursorDown(p);
            if ((gPadState[1] & 0x10) ||
                (Mod(p->horizontalRepeat, 5) == 0 && (gPadState[0] & 0x10)))
                NameEntry_MoveCursorRight(p);
            if ((gPadState[1] & 0x20) ||
                (Mod(p->horizontalRepeat, 5) == 0 && (gPadState[0] & 0x20)))
                NameEntry_MoveCursorLeft(p);
            if (gPadState[1] & 2) {
                Sound_PlaySong(0xDE);
                r = NameEntry_EraseCharacter(p);
                if (r == 1) {
                    TextEntry_CopyInitialName(p, p->name);
                    p->position = 4;
                    if (p->slot)
                        CpuSet(p->name, gGameState->playerName + 16, 8);
                    gNameEntrySavedState[0] = p->position;
                    gNameEntrySavedState[1] = p->selection;
                    gNameEntrySavedState[p->slot + 2] = 0;
                    Menu_StartFadeOut(p->fadeShift);
                    p->result = 0;
                    p->state = r;
                    break;
                }
                TextEntry_DrawName(p->name);
            } else if (gPadState[1] & 4) {
                Sound_PlaySong(0xDD);
                NameEntry_ToggleCharacterPage(p);
                break;
            } else if ((gPadState[1] & 8) && p->selection != 0x56) {
                Sound_PlaySong(0xDD);
                p->selection = 0x56;
                NameEntry_UpdateCursor(p);
                break;
            } else if ((gPadState[1] & 1) || (gPadState[1] & 8)) {
                Sound_PlaySong(0xDD);
                if (p->selection <= 0x54 && (gPadState[1] & 1)) {
                    if (NameEntry_AppendChar(p) == 1) {
                        p->selection = 0x56;
                        NameEntry_UpdateCursor(p);
                    }
                } else {
                    if (p->selection == 0x55 && (gPadState[1] & 1))
                        NameEntry_ToggleCharacterPage(p);
                    if (p->selection == 0x56) {
                        s32 count = 0;
                        u8 *str = Text_LookupString(
                            Script_ParseStringRef((const void *)gNamePlaceholderScript[0]) + 0x54);
                        s32 i;
                        u8 *c;
                        for (i = 8, c = p->name + 8; i >= 0; c -= 2, i -= 2) {
                            if ((c[0] == 0 && c[1] == 0) || (c[0] == str[0] && c[1] == str[1]))
                                count++;
                        }
                        if (count == 5) {
                            TextEntry_CopyInitialName(p, p->name);
                            p->position = 4;
                        }
                        if (p->slot == 0)
                            CpuSet(p->name, PlayerName_GetBuffer(), 8);
                        else
                            CpuSet(p->name, gGameState->playerName + 16, 8);
                        gNameEntrySavedState[0] = p->position;
                        gNameEntrySavedState[1] = p->selection;
                        Menu_StartFadeOut(p->fadeShift = 3);
                        p->state = 1;
                        p->result = 1;
                        break;
                    }
                }
                TextEntry_DrawName(p->name);
            }
            {
                s32 x = p->position << 4;
                u32 zero;
                s16 *h = &p->widgets[3].x;
                zero = 0;
                *h = x;
                p->widgets[3].y = zero;
            }
            Palette_UpdateHighlightPulse(&p->keyColors[12], &p->highlightPulseState[0]);
            Palette_UpdateHighlightPulse(&p->cursorColors[12], &p->highlightPulseState[1]);
            break;
        case 1:
            if (p->timer > (1 << p->fadeShift) || p->fadeShift == 0) {
                p->widgets[5].flags |= 1;
                p->widgets[4].flags |= 1;
                p->widgets[3].flags |= 1;
                p->widgets[2].flags |= 1;
                p->widgets[1].flags |= 1;
                p->widgets[0].flags |= 1;
                if (p->result == 0) {
                    Script_Call(gNamePlaceholderScript[3], 0);
                } else {
                    u32 v = gNamePlaceholderScript[4];
                    if (v == 0) {
                        u16 *slot;
                        slot = &gGameState->values08[0];
                        *slot = v;
                        slot = &gGameState->values08[1];
                        *slot = v;
                        slot = &gGameState->values08[2];
                        *slot = v;
                        slot = &gGameState->values08[3];
                        *slot = v;
                    }
                    Script_Call(gNamePlaceholderScript[2], 0);
                }
                Task_Kill(p);
            }
            p->timer++;
            break;
        case 2: /* unused; keeps the original case tree */
            break;
    }
}

void Anim_Remove(void *);

void NameEntry_Destroy(struct NameEntryTask *p)
{
    u16 *q = &gDispCntShadow;
    u32 m = ~3;
    *q = m & *q;
    Anim_Remove(&p->widgets[5]);
    Anim_Remove(&p->widgets[4]);
    Anim_Remove(&p->widgets[3]);
    Anim_Remove(&p->widgets[2]);
    Anim_Remove(&p->widgets[1]);
    Anim_Remove(&p->widgets[0]);
}

/* NameEntry_Initialize: reads the script's "b" (name slot) and "d" (initial name) keywords,
 * loads the slot's current name, counts its blank trailing pairs to place the cursor, then
 * sets up the keyboard backgrounds and six widgets. (guess) */

s32 Script_SeekToKeyword(s32);

s32 Script_GetValue(void);

u8 *Script_GetPc(void);

const u8 *Script_ReadString(const u8 *);

void NameEntry_UpdateCursor(struct NameEntryTask *);

void BgGfx_LoadTilemapSimple(u32, u32, u32, u32, u32);

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
    menu->characterPage = count;
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
    NameEntry_UpdateCursor(screen);
    Menu_StartFadeIn(screen->fadeShift = gNamePlaceholderScript[5]);
    return 0;
}

/* Parameters at 0x02000328 (meaning unknown). */

struct ScreenParams328 {
    u8 *kBlock, *hBlock;
    s32 o, c, p, f;
};

void ScreenParams328_SetFromScript(void)
{
    if (Script_SeekToKeyword('h'))
        (*(struct ScreenParams328 *)gNamePlaceholderScript).hBlock = Script_GetPc();
    if (Script_SeekToKeyword('k'))
        (*(struct ScreenParams328 *)gNamePlaceholderScript).kBlock = Script_GetPc();
    if (Script_SeekToKeyword('o'))
        (*(struct ScreenParams328 *)gNamePlaceholderScript).o = Script_GetValue();
    if (Script_SeekToKeyword('c'))
        (*(struct ScreenParams328 *)gNamePlaceholderScript).c = Script_GetValue();
    if (Script_SeekToKeyword('p'))
        (*(struct ScreenParams328 *)gNamePlaceholderScript).p = Script_GetValue();
    if (Script_SeekToKeyword('f'))
        (*(struct ScreenParams328 *)gNamePlaceholderScript).f = Script_GetValue();
    else
        (*(struct ScreenParams328 *)gNamePlaceholderScript).f = 5;
}

void NameEntry_Update(struct NameEntryTask *);

void NameEntry_Destroy(struct NameEntryTask *);

void *NameEntry_Create(void)
{
    void *r = Task_Create(0xb, 0x3e4);
    if (r != 0) {
        Task_SetCallbacks(r, NameEntry_Update, NameEntry_Destroy);
        if (NameEntry_Initialize(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
