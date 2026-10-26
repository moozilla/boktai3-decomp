#include "global.h"

struct HudCounters {
    u8 pad0[0x50C];
    s32 counter50C;
    u8 pad510[0x77C - 0x510];
    s32 counter77C;
    u8 pad780[0x7B0 - 0x780];
    s16 counter7B0;
};

extern struct HudCounters *gGameState;
u16 *Hud_GetTilemapCell(s32, s32, s32);
void Hud_DrawCounter7B0(s32, s32);
void Hud_DrawCounter50C(s32, s32);

void sub_081656E0(u8 *self)
{
    u16 *p = Hud_GetTilemapCell(0, 0x12, 1);
    u8 mode = self[0x42A2];

    if (mode == 0) {
        *p++ = 0xF023;
        *p++ = 0xF022;
        *p++ = 0xF024;
        p[0] = 0xF09B;
        p[1] = 0xF001;
        if (gGameState->counter7B0 > 9999)
            gGameState->counter7B0 = 9999;
        Hud_DrawCounter7B0(0x17, 1);
    } else if (mode == 1) {
        *p++ = 0xF025;
        *p++ = 0xF026;
        *p++ = 0xF022;
        p[0] = 0xF09B;
        p[1] = 0xF030;
        if (gGameState->counter50C > 9999)
            gGameState->counter50C = 9999;
        Hud_DrawCounter50C(0x17, 1);
    } else {
        *p++ = 0xF0B9;
        *p++ = 0xF0BA;
        *p++ = 0xF022;
        p[0] = 0xF0BB;
        p[1] = 0xF09B;
        if (gGameState->counter77C > 9999)
            gGameState->counter77C = 9999;
    }
}
