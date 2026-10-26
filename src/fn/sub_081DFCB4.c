#include "global.h"

struct EntryMenu {
    u8 sprites[0x34C];
    s32 selection, position;
    u8 pad354[4];
    u16 verticalHistory, horizontalHistory;
    s32 verticalRepeat, horizontalRepeat;
    s16 state, result;
    s32 timer, fadeShift;
};
extern u16 gUnk_03005260[];
extern u32 gUnk_02000358[];
extern u8 *gGameState, *gUnk_030053F8;
void Password_MoveCursorUp(void *);
void Password_MoveCursorDown(void *);
void Password_MoveCursorRight(void *);
void Password_MoveCursorLeft(void *);
void Sound_PlaySong(s32);
s32 AnimPanel_AppendSelection(void *);
s32 AnimPanel_EraseSelection(void *);
void sub_081DF1D8(void *);
void Password_BuildLabels(void *);
void sub_081DFB9C(void *);
void Menu_StartFadeOut(s32);
void Palette_UpdateHighlightPulse(void *, void *);
void AnimPanel_JoinTextRows(void *, void *);
u8 *PlayerName_GetBuffer(void);
s32 Password_DecodeEntry(void *, u32, u32, void *);
void AnimPanel_ResetSelections(void *);
void AnimPanel_SaveSelections(void *);
void Script_Call(u32, void *);
void Task_Kill(void *);
s32 Mod(s32, s32);
void Password_UpdateEntryMenu(struct EntryMenu *p)
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
            if (gUnk_03005260[0] & 0xC0)
                p->verticalHistory |= 1;
            if (gUnk_03005260[0] & 0x30)
                p->horizontalHistory |= 1;
            if (p->verticalHistory == 0xFFFF)
                p->verticalRepeat++;
            else
                p->verticalRepeat = 1;
            if (p->horizontalHistory == 0xFFFF)
                p->horizontalRepeat++;
            else
                p->horizontalRepeat = 1;
            if ((gUnk_03005260[1] & 0x40) ||
                (Mod(p->verticalRepeat, 5) == 0 && (gUnk_03005260[0] & 0x40)))
                Password_MoveCursorUp(p);
            if ((gUnk_03005260[1] & 0x80) ||
                (Mod(p->verticalRepeat, 5) == 0 && (gUnk_03005260[0] & 0x80)))
                Password_MoveCursorDown(p);
            if ((gUnk_03005260[1] & 0x10) ||
                (Mod(p->horizontalRepeat, 5) == 0 && (gUnk_03005260[0] & 0x10)))
                Password_MoveCursorRight(p);
            if ((gUnk_03005260[1] & 0x20) ||
                (Mod(p->horizontalRepeat, 5) == 0 && (gUnk_03005260[0] & 0x20)))
                Password_MoveCursorLeft(p);
            if (gUnk_03005260[1] & 1) {
                if (p->selection != 0x41) {
                    Sound_PlaySong(0xDD);
                    if (AnimPanel_AppendSelection(p)) {
                        p->selection = 0x41;
                        sub_081DF1D8(p);
                    }
                } else
                    p->state = 1;
            }
            if (gUnk_03005260[1] & 2) {
                Sound_PlaySong(0xDE);
                if (AnimPanel_EraseSelection(p)) {
                    Menu_StartFadeOut(p->fadeShift);
                    p->result = 0;
                    p->state = 2;
                }
            }
            if (gUnk_03005260[1] & 8) {
                if (p->selection != 0x41) {
                    Sound_PlaySong(0xDC);
                    p->selection = 0x41;
                    sub_081DF1D8(p);
                } else
                    p->state = 1;
            }
            if (gUnk_03005260[1] & 0x200) {
                if (p->position > 0)
                    p->position--;
                Sound_PlaySong(0xDC);
            } else if (gUnk_03005260[1] & 0x100) {
                if (p->position <= 22)
                    p->position++;
                Sound_PlaySong(0xDC);
            }
            sub_081DFB9C(p);
            Palette_UpdateHighlightPulse((u8 *)p + 0x7F0, (u8 *)p + 0x818);
            Palette_UpdateHighlightPulse((u8 *)p + 0x810, (u8 *)p + 0x819);
            break;
        case 1:
            Password_BuildLabels(p);
            AnimPanel_JoinTextRows((u8 *)p + 0x370, text);
            a = gUnk_02000358[1];
            b = gUnk_02000358[2];
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
                playerValue = *(s16 *)(gGameState + 0xA);
                *(s32 *)(gUnk_030053F8 + 0x18) = playerValue;
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
                *(u32 *)((u8 *)p + 0x2C8) |= 1;
                *(u32 *)((u8 *)p + 0x240) |= 1;
                *(u32 *)((u8 *)p + 0x1B8) |= 1;
                *(u32 *)((u8 *)p + 0x130) |= 1;
                *(u32 *)((u8 *)p + 0xA8) |= 1;
                *(u32 *)((u8 *)p + 0x20) |= 1;
                if (p->result == 0) {
                    AnimPanel_ResetSelections(p);
                    Script_Call(gUnk_02000358[5], 0);
                } else {
                    AnimPanel_SaveSelections(p);
                    if (p->result == 1)
                        Script_Call(gUnk_02000358[3], 0);
                    else
                        Script_Call(gUnk_02000358[4], 0);
                }
                Task_Kill(p);
            }
            p->timer++;
            break;
    }
}
