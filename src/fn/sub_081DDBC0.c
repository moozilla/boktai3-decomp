#include "global.h"
#include "game_state.h"
#include "ram.h"
#include "input.h"

/* NameEntry_Update: per-frame task callback. State 0 handles the d-pad (with key repeat),
 * B (erase, or cancel on an empty name), SELECT (page toggle), START (jump to End) and A
 * (type / toggle / confirm); confirming stores the name and fades out. State 1 waits for
 * the fade, hides the widgets, runs the follow-up script and kills the task. (guess) */

struct NameEntryTask {
    u8 widgets[0x348];
    s32 unk348;
    s32 selection; /* 0x34C: keyboard key; 0x55 page toggle, 0x56 End */
    s32 position;  /* 0x350: cursor position in the name */
    void *bg;
    u16 verticalHistory, horizontalHistory;
    s32 verticalRepeat, horizontalRepeat;
    s16 state, result;
    s32 timer, fadeShift;
    u32 slot;      /* 0x370 */
    u8 name[0x10]; /* 0x374 */
};

s32 Mod(s32, s32);
void NameEntry_MoveCursorUp(struct NameEntryTask *);
void NameEntry_MoveCursorDown(struct NameEntryTask *);
void NameEntry_MoveCursorRight(struct NameEntryTask *);
void NameEntry_MoveCursorLeft(struct NameEntryTask *);
void Sound_PlaySong(s32);
s32 sub_081DDB44(struct NameEntryTask *);
void TextEntry_CopyInitialName(struct NameEntryTask *, u8 *);
void TextEntry_DrawName(u8 *);
void NameEntry_ToggleCharacterPage(struct NameEntryTask *);
void NameEntry_UpdateCursor(struct NameEntryTask *);
s32 NameEntry_AppendChar(struct NameEntryTask *);
s32 Script_ParseStringRef(const void *);
u8 *Text_LookupString(s32);
u8 *PlayerName_GetBuffer(void);
void Menu_StartFadeOut(s32);
void Palette_UpdateHighlightPulse(void *, void *);
void Script_Call(u32, void *);
void Task_Kill(void *);

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
                r = sub_081DDB44(p);
                if (r == 1) {
                    TextEntry_CopyInitialName(p, p->name);
                    p->position = 4;
                    if (p->slot)
                        CpuSet(p->name, (u8 *)gGameState + 0x790, 8);
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
                            CpuSet(p->name, (u8 *)gGameState + 0x790, 8);
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
                u16 *h = (u16 *)((u8 *)p + 0x1D0);
                zero = 0;
                *h = x;
                *(u16 *)((u8 *)p + 0x1D2) = zero;
            }
            Palette_UpdateHighlightPulse((u8 *)p + 0x3B8, (u8 *)p + 0x3E0);
            Palette_UpdateHighlightPulse((u8 *)p + 0x3D8, (u8 *)p + 0x3E1);
            break;
        case 1:
            if (p->timer > (1 << p->fadeShift) || p->fadeShift == 0) {
                *(u32 *)((u8 *)p + 0x2C8) |= 1;
                *(u32 *)((u8 *)p + 0x240) |= 1;
                *(u32 *)((u8 *)p + 0x1B8) |= 1;
                *(u32 *)((u8 *)p + 0x130) |= 1;
                *(u32 *)((u8 *)p + 0xA8) |= 1;
                *(u32 *)((u8 *)p + 0x20) |= 1;
                if (p->result == 0) {
                    Script_Call(gNamePlaceholderScript[3], 0);
                } else {
                    u32 v = gNamePlaceholderScript[4];
                    if (v == 0) {
                        *(u16 *)((u8 *)gGameState + 8) = v;
                        *(u16 *)((u8 *)gGameState + 0xA) = v;
                        *(u16 *)((u8 *)gGameState + 0xC) = v;
                        *(u16 *)((u8 *)gGameState + 0xE) = v;
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
