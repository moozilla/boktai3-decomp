#include "global.h"
#include "game_state.h"
#include "render.h"
extern u16 gUnk_03005260[];
extern u32 gUnk_020003A0[];
extern u32 gUnk_0300546C, gUnk_030053E8;
void SelectionPalette_UpdatePulse(void *);
void Sound_PlaySong(s32);
void Menu_StartFadeOut(s32);
void Sound_PauseChannel10(s32);
void Script_Call(u32, void *);
void Script_SetEntryProcedure(s16 id);
void Task_Kill(void *);
#define W(o)    (*(s32 *)(p + (o)))
#define H(o)    (*(s16 *)((u8 *)gUnk_02000710 + (o)))
#define GS_W(o) (*(s32 *)((u8 *)gUnk_02000710 + (o)))
// Task update: menu input (state 0), then fade out and run the chosen script (state 1).
void GameStart_Update(u8 *p)
{
    s32 state;
    switch (state = W(0x4DC)) {
        case 0:
            SelectionPalette_UpdatePulse(p);
            if (gUnk_03005260[1] & 2) {
                Sound_PlaySong(0xDE);
                W(0x4D8) = 3;
                Menu_StartFadeOut(3);
                Sound_PauseChannel10(2);
                W(0x4DC) = 1;
                W(0x4E0) = state;
                break;
            }
            if (gUnk_03005260[1] & 1) {
                if (W(0x4CC) == 0) {
                    Sound_PlaySong(0xE0);
                    W(0x4D8) = 3;
                    Menu_StartFadeOut(3);
                    Sound_PauseChannel10(2);
                    W(0x4DC) = 1;
                    W(0x4E0) = 1;
                } else {
                    Sound_PlaySong(0xDD);
                    W(0x4D8) = state;
                    Menu_StartFadeOut(0);
                    W(0x4DC) = 1;
                    W(0x4E0) = 2;
                }
                break;
            }
            if (H(0x86E) == 0)
                break;
            if (gUnk_03005260[1] & 0x40) {
                if (--W(0x4CC) < 0)
                    W(0x4CC) = 1;
            } else if (gUnk_03005260[1] & 0x80) {
                if (++W(0x4CC) > 1)
                    W(0x4CC) = state;
            } else
                break;
            W(0x4E8) = state;
            Sound_PlaySong(0x37C);
            break;
        case 1:
            if (W(0x4D4) > (state << W(0x4D8))) {
                W(0x474) |= state;
                W(0x134) |= state;
                W(0xAC) |= state;
                W(0x24) |= state;
                if (W(0x4E0) == 0)
                    Script_Call(gUnk_020003A0[5], 0);
                else if (W(0x4E0) == 2) {
                    gUnk_0300546C = 0;
                    Script_Call(gUnk_020003A0[4], 0);
                } else if (W(0x4E0) == 1 && GS_W(0x5A4) != 0) {
                    gUnk_0300546C = 0;
                    Script_Call(gUnk_020003A0[2], 0);
                    gEnginePauseFlags = 0;
                    gUnk_030053E8 = 0x11;
                    Script_SetEntryProcedure(H(0x5A4));
                }
                Task_Kill(p);
            }
            W(0x4D4)++;
            break;
        case 2:
            break;
    }
}
