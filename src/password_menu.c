#include "password_menu.h"
#include "render.h"
#include "game_state.h"
#include "session_save.h"

void PasswordMenu_SaveSelections(struct PasswordMenu *p)
{
    s32 i = 0;
    u8 *saved = gAnimPanelSavedSelections;
    u8 *out = saved;
    u32 *values = p->selections;
    do {
        *(u8 *)(i + (u32)out) = *values++;
    } while (++i <= 23);
    saved[24] = p->position;
    saved[25] = p->selection;
}

void PasswordMenu_LoadSelections(struct PasswordMenu *p)
{
    s32 i = 0;
    u8 *saved = gAnimPanelSavedSelections;
    u8 *out = saved;
    u32 *values = p->selections;
    do {
        *values++ = *(u8 *)(i + (u32)out);
    } while (++i <= 23);
    p->position = saved[24];
    p->selection = saved[25];
}

void PasswordMenu_ResetSelections(struct PasswordMenu *p)
{
    s32 i = 0;
    u8 *saved = gAnimPanelSavedSelections;
    u32 empty = 0x41;
    u8 *out = saved;
    u32 *values = p->selections;
    do {
        *values++ = empty;
        *(u8 *)(i + (u32)out) = empty;
    } while (++i <= 23);
    p->position = 0;
    p->selection = 0;
    saved[24] = 0;
    saved[25] = 0;
}

void PasswordMenu_JoinTextRows(struct PasswordRows *text, u8 *out)
{
    s32 row = 0;
    for (row = 0; row <= 3; row++) {
        u8 *in = text->rows[row];
        s32 count = 0;
        u8 c = *in;
        if (c != 0) {
            do {
                *out = c;
                in++;
                out++;
                count++;
            } while (count <= 255 && (c = *in) != 0);
        }
    }
    *out = 0;
}

/* PasswordMenu_UpdateKeyboardCursor: NameEntry_UpdateCursor variant for a 60-cell keyboard (two 5x6 blocks) plus five buttons */

void PasswordMenu_UpdateKeyboardCursor(struct PasswordMenu *p)
{
    u8 columns[8];
    u8 rows[8];
    s16 col, row;

    memcpy(columns, gUnk_0824FD2C, 8);
    memcpy(rows, gUnk_0824FD34, 8);
    p->widgets[1].sprite.flags &= ~1;
    p->widgets[5].sprite.flags |= 1;
    if (p->selection < 60) {
        col = Mod(p->selection, 5);
        if (p->selection < 30) {
            row = Div(p->selection, 5);
            p->widgets[1].sprite.position.x = columns[col];
            p->widgets[1].sprite.position.y = rows[row];
        } else {
            row = Div(p->selection - 30, 5);
            p->widgets[1].sprite.position.x = columns[col] + 0x58;
            p->widgets[1].sprite.position.y = rows[row];
        }
    } else {
        switch (p->selection - 60) {
            case 0:
                p->widgets[1].sprite.position.x = 0xB0;
                p->widgets[1].sprite.position.y = 0;
                break;
            case 1:
                p->widgets[1].sprite.position.x = 0xC0;
                p->widgets[1].sprite.position.y = 0;
                break;
            case 2:
                p->widgets[1].sprite.position.x = 0xB0;
                p->widgets[1].sprite.position.y = 0x10;
                break;
            case 3:
                p->widgets[1].sprite.position.x = 0xC0;
                p->widgets[1].sprite.position.y = 0x10;
                break;
            case 4:
                p->widgets[1].sprite.position.x = 0xB0;
                p->widgets[1].sprite.position.y = 0x20;
                break;
            case 5:
                p->widgets[5].sprite.position.x = 0;
                p->widgets[5].sprite.position.y = 0;
                p->widgets[1].sprite.flags |= 1;
                p->widgets[5].sprite.flags &= ~1;
                break;
        }
    }
    p->widgets[1].sprite.position.y += 0x20;
}

void PasswordMenu_MoveCursorUp(struct PasswordMenu *p)
{
    switch (*(u32 *)&p->selection) {
        case 0:
            *(u32 *)&p->selection = 25;
            break;
        case 1:
            *(u32 *)&p->selection = 26;
            break;
        case 2:
            *(u32 *)&p->selection = 27;
            break;
        case 3:
            *(u32 *)&p->selection = 28;
            break;
        case 4:
            *(u32 *)&p->selection = 29;
            break;
        case 30:
            *(u32 *)&p->selection = 55;
            break;
        case 31:
            *(u32 *)&p->selection = 56;
            break;
        case 32:
            *(u32 *)&p->selection = 57;
            break;
        case 33:
            *(u32 *)&p->selection = 58;
            break;
        case 34:
            *(u32 *)&p->selection = 59;
            break;
        case 60:
            *(u32 *)&p->selection = 65;
            break;
        case 61:
            *(u32 *)&p->selection = 65;
            break;
        case 62:
            *(u32 *)&p->selection = 60;
            break;
        case 63:
            *(u32 *)&p->selection = 61;
            break;
        case 64:
            *(u32 *)&p->selection = 62;
            break;
        case 65:
            *(u32 *)&p->selection = 64;
            break;
        default:
            *(u32 *)&p->selection -= 5;
            break;
    }
    PasswordMenu_UpdateKeyboardCursor(p);
    Sound_PlaySong(0xdc);
}

void PasswordMenu_MoveCursorDown(struct PasswordMenu *p)
{
    switch (*(u32 *)&p->selection) {
        case 25:
            *(u32 *)&p->selection = 0;
            break;
        case 26:
            *(u32 *)&p->selection = 1;
            break;
        case 27:
            *(u32 *)&p->selection = 2;
            break;
        case 28:
            *(u32 *)&p->selection = 3;
            break;
        case 29:
            *(u32 *)&p->selection = 4;
            break;
        case 55:
            *(u32 *)&p->selection = 30;
            break;
        case 56:
            *(u32 *)&p->selection = 31;
            break;
        case 57:
            *(u32 *)&p->selection = 32;
            break;
        case 58:
            *(u32 *)&p->selection = 33;
            break;
        case 59:
            *(u32 *)&p->selection = 34;
            break;
        case 60:
            *(u32 *)&p->selection = 62;
            break;
        case 61:
            *(u32 *)&p->selection = 63;
            break;
        case 62:
            *(u32 *)&p->selection = 64;
            break;
        case 63:
            *(u32 *)&p->selection = 64;
            break;
        case 64:
            *(u32 *)&p->selection = 65;
            break;
        case 65:
            *(u32 *)&p->selection = 60;
            break;
        default:
            *(u32 *)&p->selection += 5;
            break;
    }
    PasswordMenu_UpdateKeyboardCursor(p);
    Sound_PlaySong(0xdc);
}

void PasswordMenu_MoveCursorLeft(struct PasswordMenu *p)
{
    switch (*(u32 *)&p->selection) {
        case 0:
            *(u32 *)&p->selection = 0x3d;
            break;
        case 5:
            *(u32 *)&p->selection = 0x3f;
            break;
        case 10:
            *(u32 *)&p->selection = 0x40;
            break;
        case 15:
            *(u32 *)&p->selection = 0x41;
            break;
        case 20:
            *(u32 *)&p->selection = 0x41;
            break;
        case 25:
            *(u32 *)&p->selection = 0x41;
            break;
        case 30:
            *(u32 *)&p->selection = 4;
            break;
        case 35:
            *(u32 *)&p->selection = 9;
            break;
        case 40:
            *(u32 *)&p->selection = 0xe;
            break;
        case 45:
            *(u32 *)&p->selection = 0x13;
            break;
        case 50:
            *(u32 *)&p->selection = 0x18;
            break;
        case 55:
            *(u32 *)&p->selection = 0x1d;
            break;
        case 60:
            *(u32 *)&p->selection = 0x22;
            break;
        case 62:
            *(u32 *)&p->selection = 0x27;
            break;
        case 64:
            *(u32 *)&p->selection = 0x2c;
            break;
        case 65:
            *(u32 *)&p->selection = 0x31;
            break;
        default:
            *(u32 *)&p->selection -= 1;
            break;
    }
    PasswordMenu_UpdateKeyboardCursor(p);
    Sound_PlaySong(0xdc);
}

void PasswordMenu_MoveCursorRight(struct PasswordMenu *p)
{
    switch (*(u32 *)&p->selection) {
        case 4:
            *(u32 *)&p->selection = 30;
            break;
        case 9:
            *(u32 *)&p->selection = 35;
            break;
        case 14:
            *(u32 *)&p->selection = 40;
            break;
        case 19:
            *(u32 *)&p->selection = 45;
            break;
        case 24:
            *(u32 *)&p->selection = 50;
            break;
        case 29:
            *(u32 *)&p->selection = 55;
            break;
        case 34:
            *(u32 *)&p->selection = 60;
            break;
        case 39:
            *(u32 *)&p->selection = 62;
            break;
        case 44:
            *(u32 *)&p->selection = 64;
            break;
        case 49:
            *(u32 *)&p->selection = 65;
            break;
        case 54:
            *(u32 *)&p->selection = 65;
            break;
        case 59:
            *(u32 *)&p->selection = 65;
            break;
        case 61:
            *(u32 *)&p->selection = 0;
            break;
        case 63:
            *(u32 *)&p->selection = 5;
            break;
        case 64:
            *(u32 *)&p->selection = 63;
            break;
        case 65:
            *(u32 *)&p->selection = 15;
            break;
        default:
            *(u32 *)&p->selection += 1;
            break;
    }
    PasswordMenu_UpdateKeyboardCursor(p);
    Sound_PlaySong(0xdc);
}

void PasswordMenu_BuildLabels(struct PasswordMenu *p)
{
    u8 *dst = p->text.rows[0];
    s32 i;
    for (i = 0; i < 24; i++) {
        s32 off = p->selections[i];
        const u8 *src;
        switch (i) {
            case 0:
                dst = p->text.rows[0];
                break;
            case 6:
                dst = p->text.rows[1];
                break;
            case 12:
                dst = p->text.rows[2];
                break;
            case 18:
                dst = p->text.rows[3];
                break;
        }
        src = Text_LookupString(Script_ParseStringRef(gSelectionMenuParameters.pBlock) + off);
        while (*src) {
            *dst = *src;
            dst++;
            src++;
        }
        *dst = 0;
    }
    Menu_EraseRect(2, 3, 0x1a, 4);
    Menu_DrawText(2, 3, p->text.rows[0]);
    Menu_DrawText(0x10, 3, p->text.rows[1]);
    Menu_DrawText(2, 5, p->text.rows[2]);
    Menu_DrawText(0x10, 5, p->text.rows[3]);
}

void PasswordMenu_RedrawSelection(struct PasswordMenu *p, s32 n)
{
    u8 tbl[4][2];
    s32 d, x, y;
    const u8 *text;
    memcpy(tbl, gUnk_0824FD3C, 8);
    d = Div(n, 6);
    x = (n - d * 6) * 2 + tbl[d][0];
    y = tbl[d][1];
    {
        u32 base = Script_ParseStringRef(gSelectionMenuParameters.pBlock);
        s32 off = n * 4;
        u8 *a = (u8 *)p->selections;
        text = Text_LookupString(base + *(u32 *)(a + off));
    }
    Menu_EraseRect(x, y, 2, 2);
    Menu_DrawText(x, y, text);
}

void PasswordMenu_UpdateEntryCursor(struct PasswordMenu *p)
{
    u8 tbl[12];
    s32 i;
    memcpy(tbl, gUnk_0824FD44, 12);
    i = p->position;
    if (i <= 11) {
        p->widgets[2].sprite.position.x = tbl[i];
        p->widgets[2].sprite.position.y = 0;
    } else {
        p->widgets[2].sprite.position.x = tbl[i - 12];
        p->widgets[2].sprite.position.y = 0x10;
    }
    p->widgets[2].sprite.position.x -= 0x80;
    p->widgets[2].sprite.position.y += 0x10;
    p->widgets[2].sprite.flags &= ~1;
}

s32 PasswordMenu_AppendSelection(struct PasswordMenu *p)
{
    s32 full;
    s32 index;
    s32 *cursor = &p->position;
    p->selections[p->position] = p->selection;
    index = *cursor;
    if (index <= 22) {
        *cursor = 1 + index;
        full = 0;
    } else {
        full = 1;
    }
    PasswordMenu_RedrawSelection(p, index);
    PasswordMenu_BuildLabels(p);
    return full;
}

s32 PasswordMenu_EraseSelection(struct PasswordMenu *p)
{
    s32 empty = 0;
    s32 index = p->position;
    u32 offset = index * 4;
    u32 *entries = p->selections;
    u32 *entry = (u32 *)((u32)entries + offset);
    u32 value = *entry;
    if (value == 0x41) {
        if (index == 0) {
            empty = 1;
        } else {
            p->position = index - 1;
            *(u32 *)((u32)entries + (p->position << 2)) = value;
        }
    } else {
        *entry = 0x41;
    }
    PasswordMenu_RedrawSelection(p, p->position);
    PasswordMenu_BuildLabels(p);
    return empty;
}

void PasswordMenu_Update(struct PasswordMenu *p)
{
    u8 text[1024];
    u8 *name;
    s32 code;
    u32 a, b;
    s32 playerValue;
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
                PasswordMenu_MoveCursorUp(p);
            if ((gPadState[1] & 0x80) || (Mod(p->verticalRepeat, 5) == 0 && (gPadState[0] & 0x80)))
                PasswordMenu_MoveCursorDown(p);
            if ((gPadState[1] & 0x10) ||
                (Mod(p->horizontalRepeat, 5) == 0 && (gPadState[0] & 0x10)))
                PasswordMenu_MoveCursorRight(p);
            if ((gPadState[1] & 0x20) ||
                (Mod(p->horizontalRepeat, 5) == 0 && (gPadState[0] & 0x20)))
                PasswordMenu_MoveCursorLeft(p);
            if (gPadState[1] & 1) {
                if (p->selection != 0x41) {
                    Sound_PlaySong(0xDD);
                    if (PasswordMenu_AppendSelection(p)) {
                        p->selection = 0x41;
                        PasswordMenu_UpdateKeyboardCursor(p);
                    }
                } else
                    p->state = 1;
            }
            if (gPadState[1] & 2) {
                Sound_PlaySong(0xDE);
                if (PasswordMenu_EraseSelection(p)) {
                    Menu_StartFadeOut(p->fadeShift);
                    p->result = 0;
                    p->state = 2;
                }
            }
            if (gPadState[1] & 8) {
                if (p->selection != 0x41) {
                    Sound_PlaySong(0xDC);
                    p->selection = 0x41;
                    PasswordMenu_UpdateKeyboardCursor(p);
                } else
                    p->state = 1;
            }
            if (gPadState[1] & 0x200) {
                if (p->position > 0)
                    p->position--;
                Sound_PlaySong(0xDC);
            } else if (gPadState[1] & 0x100) {
                if (p->position <= 22)
                    p->position++;
                Sound_PlaySong(0xDC);
            }
            PasswordMenu_UpdateEntryCursor(p);
            Palette_UpdateHighlightPulse(&p->keyColors[12], &p->keyPulse);
            Palette_UpdateHighlightPulse(&p->cursorColors[12], &p->cursorPulse);
            break;
        case 1:
            PasswordMenu_BuildLabels(p);
            PasswordMenu_JoinTextRows(&p->text, text);
            a = gSelectionMenuParameters.PBlock;
            b = gSelectionMenuParameters.NBlock;
            name = PlayerName_GetBuffer();
            code = Password_DecodeEntry(text, a, b, name);
            {
                u8 *name = PlayerName_GetBuffer();
                s32 i;
                i = 0;
                do {
                    u32 ch = *name;
                    if (ch == 0 && i <= 8)
                        name[1] = ch;
                    name++;
                    i++;
                } while (i <= 9);
            }
            if (code == 0) {
                Sound_PlaySong(0x296);
                playerValue = (s16)gGameState->values08[1];
                ((struct SessionSave *)gSessionState)->region = playerValue;
                Menu_StartFadeOut(p->fadeShift);
                p->result = 1;
                p->state = 2;
            } else {
                Sound_PlaySong(0x192);
                Menu_StartFadeOut(p->fadeShift);
                p->result = 2;
                p->state = 2;
            }
            break;
        case 2:
            if (p->timer > (1 << p->fadeShift) || p->fadeShift == 0) {
                p->widgets[5].sprite.flags |= 1;
                p->widgets[4].sprite.flags |= 1;
                p->widgets[3].sprite.flags |= 1;
                p->widgets[2].sprite.flags |= 1;
                p->widgets[1].sprite.flags |= 1;
                p->widgets[0].sprite.flags |= 1;
                if (p->result == 0) {
                    PasswordMenu_ResetSelections(p);
                    Script_Call(gSelectionMenuParameters.c, 0);
                } else {
                    PasswordMenu_SaveSelections(p);
                    if (p->result == 1)
                        Script_Call(gSelectionMenuParameters.s, 0);
                    else
                        Script_Call(gSelectionMenuParameters.f, 0);
                }
                Task_Kill(p);
            }
            p->timer++;
            break;
    }
}

void PasswordMenu_Destroy(struct PasswordMenu *p)
{
    u16 *q = &gDispCntShadow;
    u32 m = ~3;
    *q = m & *q;
    Anim_Remove(&p->widgets[5].sprite);
    Anim_Remove(&p->widgets[4].sprite);
    Anim_Remove(&p->widgets[2].sprite);
    Anim_Remove(&p->widgets[3].sprite);
    Anim_Remove(&p->widgets[1].sprite);
    Anim_Remove(&p->widgets[0].sprite);
}

s32 PasswordMenu_Initialize(struct PasswordMenu *s)
{
    void *secondResource;
    u32 *selection = (u32 *)(&s->selection);
    u32 zero;
    u32 size;
    u32 one;
    u32 layer;
    void *resource, *background;
    void *palette;
    void *graphics;
    void *firstResource;
    u32 resourceBank;
    s32 offA;
    s32 offB;
    u32 *verticalRepeat;
    u32 *ptr;
    u32 *horizontalRepeat;
    u16 *keyPaletteBank;
    u16 *cursorPaletteBank;
    u16 keyPaletteIndex;
    u16 cursorPaletteIndex;
    u8 *verticalHistory;
    *selection = (zero = 0);
    offA = (u32) & ((struct PasswordMenu *)0)->unknown348;
    *(u32 *)((u8 *)s + offA) = zero;
    offB = (u32) & ((struct PasswordMenu *)0)->position;
    *(u32 *)((u8 *)s + offB) = zero;
    offA = (u32) & ((struct PasswordMenu *)0)->verticalHistory;
    verticalHistory = (u8 *)s + offA;
    *(u16 *)verticalHistory = zero;
    offB = (u32) & ((struct PasswordMenu *)0)->horizontalHistory;
    *(u16 *)((u8 *)s + offB) = zero;
    verticalRepeat = (u32 *)(&s->verticalRepeat);
    offB = (u32) & ((struct PasswordMenu *)0)->horizontalRepeat;
    horizontalRepeat = (u32 *)((u8 *)s + offB);
    *horizontalRepeat = (one = 1);
    *verticalRepeat = one;
    *(u16 *)(&s->state) = zero;
    PasswordMenu_LoadSelections(s);
    if (gSelectionMenuParameters.S) {
        *selection = 0x41;
    }
    Bg_ClearTilemap(0);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, zero);
    Bg_ClearTilemap(2);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, zero);
    Bg_ClearTilemap(3);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, zero);
    background = ScriptProc_Start(0xC091, 0x3536);
    s->background = background;
    layer = 8;
    Bg_SetupLayers(2, 0, background, 0, zero, one, &layer);
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, zero);
    palette = ((struct PasswordMenuPalette *)ScriptProc_Start(0x92B3, 0x204))->colors;
    CpuSet(palette, gBgPaletteBuffer, size = 0x100);
    resourceBank = 0xCB05;
    resource = ScriptProc_Start(resourceBank, 0xE2AB);
    firstResource = &s->widgets[0].resource;
    ResourceHeader_Relocate(firstResource, resource);
    Widget_InitLinked(&s->widgets[0].sprite, firstResource, 0x39, 0x10, zero, zero, zero, zero);
    secondResource = (u8 *)&s->widgets[1].resource;
    ResourceHeader_Relocate(secondResource, resource);
    Widget_InitLinked(&s->widgets[1].sprite, secondResource, 0x16, 0x10, zero, zero, zero, zero);
    graphics = &s->widgets[2].resource;
    ResourceHeader_Relocate(graphics, resource);
    Widget_InitLinked(&s->widgets[2].sprite, graphics, 0x18, 0x10, zero, zero, zero, zero);
    graphics = &s->widgets[3].resource;
    ResourceHeader_Relocate(graphics, resource);
    Widget_InitLinked(&s->widgets[3].sprite, graphics, 0x32, 0x10, zero, zero, zero, zero);
    {
        u32 *save = (u32 *)(&s->savedKeyPalette);
        u32 old;
        ptr = (u32 *)&s->widgets[1].sprite.palette;
        old = *ptr;
        *save = old;
        graphics = &s->keyColors;
        CpuSet((void *)old, graphics, 0x10);
        *ptr = (u32)graphics;
    }
    *(void **)(&s->widgets[2].sprite.palette) = graphics;
    keyPaletteBank = (u16 *)(&s->widgets[1].sprite.paletteBank);
    keyPaletteIndex = 0xE000;
    *keyPaletteBank = keyPaletteIndex;
    keyPaletteBank = (u16 *)(&s->widgets[2].sprite.paletteBank);
    {
        u32 palette = 0xE002;
        *keyPaletteBank = palette;
    }
    resource = ScriptProc_Start(resourceBank, 0x530D);
    offA = (u32) & ((struct PasswordMenu *)0)->widgets[4].resource;
    graphics = (u8 *)s + offA;
    ResourceHeader_Relocate(graphics, resource);
    offB = (u32) & ((struct PasswordMenu *)0)->widgets[4].sprite;
    Widget_InitLinked((u8 *)s + offB, graphics, 0x8C, 0x10, one, zero, zero, zero);
    offB = (u32) & ((struct PasswordMenu *)0)->widgets[5].resource;
    graphics = (u8 *)s + offB;
    ResourceHeader_Relocate(graphics, resource);
    Widget_InitLinked(&s->widgets[5].sprite, graphics, 0x8D, 0x10, zero, zero, zero, zero);
    *(u32 *)(&s->widgets[4].sprite.flags) &= ~1;
    *(u32 *)(&s->widgets[5].sprite.flags) |= one;
    {
        u32 *save = (u32 *)(&s->savedCursorPalette);
        u32 old;
        ptr = (u32 *)(&s->widgets[5].sprite.palette);
        old = *ptr;
        *save = old;
        graphics = &s->cursorColors;
        CpuSet((void *)old, graphics, 0x10);
        *ptr = (u32)graphics;
    }
    cursorPaletteBank = (u16 *)(&s->widgets[5].sprite.paletteBank);
    cursorPaletteIndex = 0xE010;
    *cursorPaletteBank = cursorPaletteIndex;
    s->fadeShift = zero;
    Menu_StartFadeIn(0);
    PasswordMenu_UpdateKeyboardCursor(s);
    PasswordMenu_BuildLabels(s);
    return 0;
}

/* Script parameters for entry decoding and success/failure callbacks. */

void PasswordMenu_SetParametersFromScript(void)
{
    struct SelectionMenuParameters *p = &gSelectionMenuParameters;
    p->S = 0;
    if (Script_SeekToKeyword('p'))
        p->pBlock = Script_GetPc();
    if (Script_SeekToKeyword('P'))
        p->PBlock = Script_GetPc();
    if (Script_SeekToKeyword('N'))
        p->NBlock = Script_GetPc();
    if (Script_SeekToKeyword('s'))
        p->s = Script_GetValue();
    if (Script_SeekToKeyword('f'))
        p->f = Script_GetValue();
    if (Script_SeekToKeyword('c'))
        p->c = Script_GetValue();
    if (Script_SeekToKeyword('S'))
        p->S = 1;
}

void *PasswordMenu_Create(void)
{
    void *r = Task_Create(0xb, sizeof(struct PasswordMenu));
    if (r != 0) {
        Task_SetCallbacks(r, PasswordMenu_Update, PasswordMenu_Destroy);
        if (PasswordMenu_Initialize(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}

// Names are tentative, based on the static state transitions and memory writes.

// The original callback leaves r0 unspecified; preserve its non-void return ABI.

u32 PasswordDisplay_Update(struct PasswordDisplay *p)
{
    switch (p->state) {
        case 0:
            if (p->frame <= 20 && !(p->frame & 1))
                Sound_PlaySong(628);
            if (p->frame > 59 && (gPadState[1] & 8)) {
                Sound_PlaySong(221);
                p->state = 1;
                Menu_StartFadeOut(p->fadeShift);
            }
            break;
        case 1:
            if (p->fadeTimer > (1 << p->fadeShift) || p->fadeShift == 0) {
                if (gScriptMessageParameters.value)
                    Script_Call(gScriptMessageParameters.value, 0);
                Task_Kill(p);
            } else {
                p->fadeTimer++;
            }
            break;
    }
    p->frame++;
}

s32 PasswordDisplay_Destroy(struct PasswordDisplay *p)
{
    Anim_Remove(&p->widget);
}

s32 PasswordDisplay_Initialize(struct PasswordDisplay *p)
{
    u8 *src;
    s32 row, i, j, code;
    u8 *out;
    s32 *w;
    u32 size;

    EncodedRecord_EncodeFields(p->encoded);
    src = p->encoded;
    row = 0;
    do {
        out = p->text.rows[row];
        i = 0;
        do {
            code = *src++ << 1;
            if (code > 0x59) {
                j = 0;
                do {
                    *out++ = gUnk_086136D8[j];
                    j++;
                } while (j <= 7);
            }
            {
                u8 *m = gScriptMessageParameters.text + code;
                *out++ = m[0];
                *out++ = m[1];
            }
            if (code > 0x59) {
                j = 0;
                do {
                    *out++ = gUnk_086136E4[j];
                    j++;
                } while (j <= 8);
            }
            i++;
        } while (i <= 5);
        *out = 0;
        row++;
    } while (row <= 3);

    Bg_ClearTilemap(0);
    BgGfx_LoadTilemapSimple(0, 0, 0, 0, 0);
    Bg_ClearTilemap(2);
    BgGfx_LoadTilemapSimple(2, 0, 0, 0, 0);
    Bg_ClearTilemap(3);
    {
        u8 *res = ScriptProc_Start(0xc091, 0x3536);
        p->background = res;
        size = 12;
        Bg_SetupLayers(2, 0, res, 0, 0, 1, &size);
    }
    BgGfx_LoadTilemapSimple(3, 0, 0, 0, 0);
    CpuSet(((struct PasswordMenuPalette *)ScriptProc_Start(0x92b3, 0x204))->colors,
           gBgPaletteBuffer, 0x100);
    ResourceHeader_Relocate(&p->resource, ScriptProc_Start(0xcb05, 0xe2ab));
    Widget_InitLinked(&p->widget, &p->resource, 0x39, 0x10, 0, 0, 0, 0);

    w = &p->windows[0];
    if ((*w = TextWindow_Create(2, 5, 28, 2)) < 0)
        return -1;
    for (i = 0; i <= 3; i++)
        TextWindow_SetString(*w, i, p->text.rows[i]);
    TextWindow_SetScript(*w, gScriptMessageParameters.pc);
    TextWindow_Refresh(*w, 0);
    TextWindow_StartPlayback(*w);

    w = &p->windows[1];
    if ((*w = TextWindow_Create(6, 17, 28, 1)) < 0)
        return -1;
    TextWindow_SetScript(*w, gScriptMessageParameters.pc);
    TextWindow_Refresh(*w, 1);
    TextWindow_StartPlayback(*w);

    w = &p->windows[2];
    if ((*w = TextWindow_Create(2, 11, 28, 2)) < 0)
        return -1;
    TextWindow_SetScript(*w, gScriptMessageParameters.pc);
    TextWindow_Refresh(*w, 2);
    TextWindow_StartPlayback(*w);

    p->fadeShift = 0;
    Menu_StartFadeIn(0);
    return 0;
}

/* Script: text from string -c (Text_LookupString) into gScriptMessageParameters, with
   -d (script position) and -e (value) (tentative). */

void PasswordDisplay_SetParametersFromScript(void)
{
    u8 *pc = 0;
    if (Script_SeekToKeyword('c'))
        pc = Script_GetPc();
    gScriptMessageParameters.text = Text_LookupString(Script_ParseStringRef(pc));
    if (Script_SeekToKeyword('d'))
        gScriptMessageParameters.pc = Script_GetPc();
    if (Script_SeekToKeyword('e'))
        gScriptMessageParameters.value = Script_GetValue();
}

void *PasswordDisplay_Create(void)
{
    void *r = Task_Create(0xb, sizeof(struct PasswordDisplay));
    if (r != 0) {
        Task_SetCallbacks(r, PasswordDisplay_Update, PasswordDisplay_Destroy);
        if (PasswordDisplay_Initialize(r) < 0) {
            Task_Kill(r);
            return 0;
        }
    }
    return r;
}
