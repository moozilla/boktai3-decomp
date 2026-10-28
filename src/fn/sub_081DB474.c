// MATCHING_CONCESSION: register-pinning
#include "global.h"
#include "menu_parameters.h"
struct Rect {
    u32 xy;
    u16 width, height;
};
struct Message {
    u32 count : 16;
    u32 unused : 16;
    void *values;
};
extern u16 gUnk_03005260[];
void Sound_PlaySong(s32);
void Menu_StartFadeOut(s32);
void Menu_EraseRect(s32, s32, s32, s32);
void Menu_DrawStringRef(s32, s32, u32, s32);
void Menu_SetTextRect(void *, void *);
void Palette_UpdateHighlightPulse(void *, void *);
void Script_Call(u32, void *);
void Task_Kill(void *);
#define W(o) (*(s32 *)(p + (o)))
void Options_UpdateScreen(u8 *p)
{
    s32 value;
    s32 state;
    struct Message message;
    struct Rect rect;
    message.count = 1;
    message.values = &value;
    switch (state = W(0x2CC)) {
        case 0:
            if (gUnk_03005260[1] & 1) {
                if (W(0x1C) == 0) {
                    Sound_PlaySong(0xDD);
                    W(0x2D8) = state;
                    Menu_StartFadeOut(0);
                    W(0x2CC) = 1;
                    W(0x2D0) = 1;
                    break;
                } else if (W(0x1C) == 1) {
                    Sound_PlaySong(0xDD);
                    W(0x2D8) = state;
                    Menu_StartFadeOut(0);
                    W(0x2CC) = 1;
                    W(0x2D0) = 2;
                    break;
                } else if (W(0x1C) == 2) {
                    Sound_PlaySong(0xDD);
                    W(0x2D8) = 2;
                    Menu_StartFadeOut(2);
                    W(0x2CC) = 1;
                    W(0x2D0) = 3;
                    break;
                }
            }
            if (gUnk_03005260[1] & 2) {
                Sound_PlaySong(0xDE);
                W(0x2D8) = 3;
                Menu_StartFadeOut(3);
                W(0x2CC) = 1;
                W(0x2D0) = 0;
                break;
            }
            if (gUnk_03005260[1] & 0x40) {
                Sound_PlaySong(0xDC);
                if (--W(0x1C) < 0)
                    W(0x1C) = 2;
            }
            if (gUnk_03005260[1] & 0x80) {
                Sound_PlaySong(0xDC);
                if (++W(0x1C) > 2)
                    W(0x1C) = 0;
            }
            if (W(0x1C) == 2) {
                if (gUnk_03005260[1] & 0x20) {
                    Sound_PlaySong(0xDC);
                    if (--W(0x20) < 0)
                        W(0x20) = 8;
                    Menu_EraseRect(9, 12, 18, 2);
                    Menu_DrawStringRef(9, 12, gChoiceMenuParameters.eBlock, W(0x20));
                }
                if (gUnk_03005260[1] & 0x10) {
                    Sound_PlaySong(0xDC);
                    if (++W(0x20) > 8)
                        W(0x20) = 0;
                    Menu_EraseRect(9, 12, 18, 2);
                    Menu_DrawStringRef(9, 12, gChoiceMenuParameters.eBlock, W(0x20));
                }
                gChoiceMenuParameters.n2 = W(0x20);
            }
            {
                u32 mask = 0xFFFF0000;
                asm volatile("" : : "r"(mask));
                rect.xy = (W(0x1C) << 21) + 0x200000;
                rect.width = 0;
            }
            Menu_SetTextRect(p + 0x134, &rect);
            W(0x13C) &= ~1;
            Palette_UpdateHighlightPulse(p + 0x2F8, p + 0x300);
            if (W(0x1C) == 2) {
                W(0x1C4) &= ~1;
                W(0x24C) &= ~1;
            } else {
                W(0x1C4) |= 1;
                W(0x24C) |= 1;
            }
            break;
        case 1:
            if (W(0x2D4) > (state << W(0x2D8))) {
                W(0x24C) |= state;
                W(0x1C4) |= state;
                W(0x13C) |= state;
                W(0xB4) |= state;
                W(0x2C) |= state;
                Script_Call(gChoiceMenuParameters.p, 0);
                if (W(0x2D0) == 0)
                    Script_Call(gChoiceMenuParameters.c, 0);
                else if (W(0x2D0) == 1)
                    Script_Call(gChoiceMenuParameters.s, 0);
                else if (W(0x2D0) == 2)
                    Script_Call(gChoiceMenuParameters.r, 0);
                else {
                    value = W(0x20);
                    Script_Call(gChoiceMenuParameters.x, &message);
                }
                Task_Kill(p);
            }
            W(0x2D4)++;
            break;
        case 2:
            break;
    }
}
