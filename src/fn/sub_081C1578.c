#include "global.h"
#include "game_state.h"

s32 Script_SeekToKeyword(u32);
s32 Script_GetValue(void);
u32 Script_GetPc(void);
void RecordDisplay_LoadBackground(u8 *);
void RecordDisplay_LoadPalette(u8 *);
void RecordDisplay_LoadAnimationResources(u8 *);
s32 Div(s32, s32);
s32 Mod(s32, s32);
void PlayStats_SetState(u8 *, s32);

#define GS_W(o) (*(s32 *)((u8 *)gGameState + (o)))

s32 sub_081C1578(u8 *p)
{
    s32 hours, minutes, seconds;
    s32 value, i;
    u8 c;

    if (Script_SeekToKeyword('e'))
        *(s32 *)(p + 0x64) = Script_GetValue();
    *(s32 *)(p + 0x60) = 0;
    *(s32 *)(p + 0x68) = 0;
    *(s32 *)(p + 0x6c) = 0;
    *(s32 *)(p + 0x70) = 0;
    *(s32 *)(p + 0x74) = 0;
    *(s32 *)(p + 0x78) = -1;
    RecordDisplay_LoadBackground(p);
    RecordDisplay_LoadPalette(p);
    RecordDisplay_LoadAnimationResources(p);
    hours = Div(GS_W(0x614), 216000);
    minutes = Div(GS_W(0x614), 3600) - hours * 60;
    seconds = Div(GS_W(0x614), 60) - hours * 3600 - minutes * 60;
    if (hours > 99) {
        p[0x7c] = '9';
        p[0x7d] = '9';
        p[0x7f] = '5';
        p[0x80] = '9';
        p[0x82] = '5';
        p[0x83] = '9';
    } else {
        c = Mod(hours, 10) + '0';
        p[0x7d] = c;
        hours = Div(hours, 10);
        c = Mod(hours, 10) + '0';
        p[0x7c] = c;
        c = Mod(minutes, 10) + '0';
        p[0x80] = c;
        minutes = Div(minutes, 10);
        c = Mod(minutes, 10) + '0';
        p[0x7f] = c;
        c = Mod(seconds, 10) + '0';
        p[0x83] = c;
        seconds = Div(seconds, 10);
        c = Mod(seconds, 10) + '0';
        p[0x82] = c;
    }
    {
        u8 *colon = p + 0x81;
        u8 zero = 0;
        *colon = ':';
        colon -= 3;
        *colon = ':';
        p[0x84] = zero;
    }
    value = GS_W(0x520);
    if (value > 99999999)
        value = 99999999;
    for (i = 9; i >= 0; i--) {
        if (value == 0) {
            if (i == 9)
                p[0x8e] = '0';
            else
                p[0x85 + i] = ' ';
        } else if (i == 2 || i == 6) {
            p[0x85 + i] = ',';
        } else {
            c = Mod(value, 10) + '0';
            p[0x85 + i] = c;
            value = Div(value, 10);
        }
    }
    p[0x8f] = 0;
    PlayStats_SetState(p, 0);
    if (Script_SeekToKeyword('s'))
        *(u32 *)(p + 0x458) = Script_GetPc();
    if (Script_SeekToKeyword('T'))
        *(u32 *)(p + 0x45c) = Script_GetPc();
    if (Script_SeekToKeyword('p'))
        *(u32 *)(p + 0x460) = Script_GetPc();
    return 0;
}
