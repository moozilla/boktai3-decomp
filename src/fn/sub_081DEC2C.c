#include "global.h"
#include "menu_parameters.h"

struct DateMenu {
    u8 graphics[0x6D8];
    s32 selection, timer, fadeShift, state, result;
    s32 year, month, day, hour, minute;
    s32 oldYear, oldMonth, oldDay, oldHour, oldMinute;
};
extern u16 gUnk_03005260[];
extern u8 gUnk_03005430[];
void Sound_PlaySong(s32);
void AnimPanel_HandleClockInput(void *);
void SunClock_SetDateTime(s32, s32, s32, s32, s32, s32, s32);
void Time_CalculateSunriseSunset(s32, s32, s32);
void Menu_StartFadeOut(s32);
void Password_UpdateCursorPanel(void *);
void Palette_UpdateHighlightPulse(void *, void *);
void MenuSprites_SetFlagBits(void *);
void Script_Call(u32, void *);
void Task_Kill(void *);
void Time_UpdateDateMenu(struct DateMenu *p)
{
    switch (p->state) {
        case 2:
            break;
        case 0:
            p->oldYear = p->year;
            p->oldMonth = p->month;
            p->oldDay = p->day;
            p->oldHour = p->hour;
            p->oldMinute = p->minute;
            if (gUnk_03005260[1] & 0x20) {
                if (p->selection <= 0)
                    p->selection = 4;
                else
                    p->selection--;
                Sound_PlaySong(0xDC);
            } else if (gUnk_03005260[1] & 0x10) {
                if (p->selection > 3)
                    p->selection = 0;
                else
                    p->selection++;
                Sound_PlaySong(0xDC);
            }
            AnimPanel_HandleClockInput(p);
            if (gUnk_03005260[1] & 8) {
                Sound_PlaySong(0xDD);
                SunClock_SetDateTime(p->year, p->month, p->day, p->hour, p->minute, 0, 1);
                Time_CalculateSunriseSunset(0x23A666, 0x8BC000, 0);
                gUnk_03005430[9] = 0;
                Menu_StartFadeOut(p->fadeShift);
                p->state = 1;
                p->result = 1;
            }
            Password_UpdateCursorPanel(p);
            Palette_UpdateHighlightPulse((u8 *)p + 0x734, (u8 *)p + 0x73C);
            break;
        case 1:
            if (p->timer > (1 << p->fadeShift)) {
                MenuSprites_SetFlagBits(p);
                if (p->result == 0)
                    Script_Call(gDateMenuParameters.cancelProc, 0);
                else
                    Script_Call(gDateMenuParameters.confirmProc, 0);
                Task_Kill(p);
            }
            p->timer++;
            break;
    }
}
