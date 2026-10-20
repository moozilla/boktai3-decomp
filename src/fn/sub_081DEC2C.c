#include "global.h"
/* AnimPanel_UpdateClockSetup: clock-setting screen update; moves the field cursor, edits the value, commits the date/time on Start and runs the exit script after the fade */
#define AnimPanel_UpdateClockSetup sub_081DEC2C
#define AnimPanel_HandleClockInput sub_081DEA60

/* Tentative clock-setting layout of the AnimPanel task. */
struct AnimPanelClock {
    u8 pad0[0x6D8];
    s32 cursor;
    s32 fadeTimer;
    s32 fadeSpeed;
    s32 state;
    s32 confirmed;
    s32 year;
    s32 month;
    s32 day;
    s32 hour;
    s32 minute;
    s32 prevYear;
    s32 prevMonth;
    s32 prevDay;
    s32 prevHour;
    s32 prevMinute;
    u8 pad714[0x734 - 0x714];
    u8 pulse[8];
    u8 pulseState[4];
};

struct AnimPanelKeys {
    u16 held;
    u16 pressed;
};
extern struct AnimPanelKeys gUnk_03005260;
extern u32 gUnk_02000340[];
extern u8 gUnk_03005430[];
void Sound_PlaySong(u32);
void AnimPanel_HandleClockInput(struct AnimPanelClock *);
void SunClock_SetDateTime(s32 year, s32 month, s32 day, s32 hour, s32 minute, s32, s32);
void Time_CalculateSunriseSunset(s32, s32, s32);
void Menu_StartFadeOut(u32);
void sub_081DE780(struct AnimPanelClock *);
void Palette_UpdateHighlightPulse(void *, void *);
void MenuSprites_SetFlagBits(struct AnimPanelClock *);
s32 Script_Call(u32, void *);
void Task_Kill(void *);

void AnimPanel_UpdateClockSetup(struct AnimPanelClock *p)
{
    switch (p->state) {
        case 0:
            p->prevYear = p->year;
            p->prevMonth = p->month;
            p->prevDay = p->day;
            p->prevHour = p->hour;
            p->prevMinute = p->minute;
            if (gUnk_03005260.pressed & 0x20) {
                if (p->cursor <= 0)
                    p->cursor = 4;
                else
                    p->cursor--;
                Sound_PlaySong(0xDC);
            } else if (gUnk_03005260.pressed & 0x10) {
                if (p->cursor > 3)
                    p->cursor = 0;
                else
                    p->cursor++;
                Sound_PlaySong(0xDC);
            }
            AnimPanel_HandleClockInput(p);
            if (gUnk_03005260.pressed & 8) {
                Sound_PlaySong(0xDD);
                SunClock_SetDateTime(p->year, p->month, p->day, p->hour, p->minute, 0, 1);
                Time_CalculateSunriseSunset(0x23A666, 0x8BC000, 0);
                gUnk_03005430[9] = 0;
                Menu_StartFadeOut(p->fadeSpeed);
                p->state = 1;
                p->confirmed = 1;
            }
            sub_081DE780(p);
            Palette_UpdateHighlightPulse(p->pulse, p->pulseState);
            break;
        case 1:
            if (p->fadeTimer > (1 << p->fadeSpeed)) {
                MenuSprites_SetFlagBits(p);
                if (p->confirmed == 0)
                    Script_Call(gUnk_02000340[3], 0);
                else
                    Script_Call(gUnk_02000340[2], 0);
                Task_Kill(p);
            }
            p->fadeTimer++;
            break;
        case 2:
            break;
    }
}
